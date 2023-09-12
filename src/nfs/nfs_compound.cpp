/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#include <string>

#include "dnfsd/dnfs_config.h"
#include "dnfsd/dnfs_meta_data.h"
#include "nfs/nfs_base.h"
#include "nfs/nfs_utils.h"
#include "nfs/nfs_auth.h"
#include "nfs/nfs_exports.h"
#include "nfs/nfs_file_handle.h"
#include "nfs/nfs_compound_base.h"
#include "utils/common_utils.h"
#include "log/log.h"

using namespace std;

#define MODULE_NAME "DNFS"

#define nfsop4_to_str(nfsop4) \
    (nfsop4 < 0 || nfsop4 >= NFS4_OP_LAST_ONE) ? op_names_v4[0] : op_names_v4[nfsop4]

/**
 *
 * @brief get the opcodes of compound
 *
 *  @param[in]  data               Compound request's data
 *  @param[out] opcodes            all opcodes in Compound
 *  @param[in]  opcodes_array_len  length of opcodes array
 *
 * @retval number of opcode in compound.
 */
uint32_t get_nfs4_opcodes(compound_data_t *data, nfs_opnum4 *opcodes,
                          uint32_t opcodes_array_len)
{
    uint32_t i = 0;

    assert(opcodes_array_len >= data->argarray_len);

    for (i = 0; i < data->argarray_len; i++)
        opcodes[i] = data->argarray[i].argop;

    return data->argarray_len;
}


/**
 * @brief Free a compound data structure
 *
 * This function frees one compound data structure.
 *
 * @param[in,out] data The compound_data_t to be freed
 *
 */
void compound_data_Free(compound_data_t *data)
{
    if (data == NULL)
        return;

    /* Release refcounted cache entries. A note on current_ds and saved_ds,
     * If both are in use and the same, it will be released during
     * set_saved_entry since set_current_entry will have set current_ds to
     * NULL. If both are non-NULL and different, current_ds will be
     * release by set_current_entry and saved_ds will be released by
     * set_saved_entry.
     */
    set_current_entry(data, NULL);
    set_saved_entry(data, NULL);

    gsh_free(data->tagname);

    if (data->session) {
        if (data->slotid != UINT32_MAX) {
            nfs41_session_slot_t *slot;

            /* Release the slot if in use */
            slot = &data->session->fc_slots[data->slotid];
            PTHREAD_MUTEX_unlock(&slot->slot_lock);
        }

        dec_session_ref(data->session);
        data->session = NULL;
    }

    /* Release SavedFH reference to export. */
    if (data->saved_export) {
        put_gsh_export(data->saved_export);
        data->saved_export = NULL;
    }

    /* If there was a saved_pnfs_ds is present, release reference. */
    if (data->saved_pnfs_ds != NULL) {
        pnfs_ds_put(data->saved_pnfs_ds);
        data->saved_pnfs_ds = NULL;
    }

    if (data->currentFH.nfs_fh4_val != NULL)
        gsh_free(data->currentFH.nfs_fh4_val);

    if (data->savedFH.nfs_fh4_val != NULL)
        gsh_free(data->savedFH.nfs_fh4_val);

    gsh_free(data);
}

void complete_nfs4_compound(compound_data_t *data, int status,
                            enum nfs_req_result result)
{
    COMPOUND4res *res_compound4;

    res_compound4 = &data->res->res_compound4_extended->res_compound4;

    server_stats_compound_done(data->argarray_len, status);

    /* Complete the reply, in particular, tell where you stopped if
     * unsuccessful COMPOUD
     */
    res_compound4->status = status;

    /* Manage session's DRC: keep NFS4.1 replay for later use, but don't
     * save a replayed result again.
     */
    if (data->sa_cachethis) {
        /* Pointer has been set by nfs4_op_sequence and points to slot
         * to cache result in.
         */
        LogFullDebug(COMPONENT_SESSIONS,
                     "Save result in session replay cache %p sizeof nfs_res_t=%d",
                     data->slot->cached_result, (int)sizeof(nfs_res_t));

        /* Save the result pointer in the slot cache (the correct slot
         * is pointed to by data->cached_result).
         */
        data->slot->cached_result = data->res->res_compound4_extended;

        /* record the latest request. */
        set_slot_last_req(data);

        /* Take a reference to indicate that this reply is cached. */
        atomic_inc_int32_t(&data->slot->cached_result->res_refcnt);
    } else if (data->minorversion > 0 &&
               result != NFS_REQ_REPLAY &&
               data->argarray[0].argop == NFS4_OP_SEQUENCE &&
               data->slot != NULL) {
        /* We need to cache an "uncached" response. The length is
         * 1 if only one op processed, otherwise 2. */
        struct COMPOUND4res *c_res;
        u_int resarray_len =
                res_compound4->resarray.resarray_len == 1 ? 1 : 2;
        struct nfs_resop4 *res0;

        /* If the slot happened to be in use, release it. */
        release_slot(data->slot);

        /* Allocate (and zero) a new COMPOUND4res_extended */
        data->slot->cached_result =
                gsh_calloc(1, sizeof(*data->slot->cached_result));

        /* record the latest request. */
        set_slot_last_req(data);

        /* Take initial reference to response. */
        data->slot->cached_result->res_refcnt = 1;

        c_res = &data->slot->cached_result->res_compound4;

        c_res->resarray.resarray_len = resarray_len;
        c_res->resarray.resarray_val =
                gsh_calloc(resarray_len, sizeof(struct nfs_resop4));
        copy_tag(&c_res->tag, &res_compound4->tag);
        res0 = c_res->resarray.resarray_val;

        /* Copy the sequence result. */
        *res0 = res_compound4->resarray.resarray_val[0];
        c_res->status = res0->nfs_resop4_u.opillegal.status;

        if (resarray_len == 2) {
            struct nfs_resop4 *res1 = res0 + 1;

            /* Shallow copy response since we will override any
             * resok or any negative response that might have
             * allocated data.
             */
            *res1 = res_compound4->resarray.resarray_val[1];

            /* Override NFS4_OK and NFS4ERR_DENIED. We MUST override
             * NFS4_OK since we aren't caching a full response and
             * we MUST override NFS4ERR_DENIED because LOCK and
             * LOCKT allocate data that we did not deep copy.
             *
             * If any new operations are added with dynamically
             * allocated data associated with a non-NFS4_OK
             * status are added in some future minor version, they
             * will likely need special handling here also.
             *
             * Note that we COULD get fancy and if we had a 2 op
             * compound that had an NFS4_OK status and no dynamic
             * data was allocated then go ahead and cache the
             * full response since it wouldn't take any more
             * memory. However, that would add a lot more special
             * handling here.
             */
            if (res1->nfs_resop4_u.opillegal.status == NFS4_OK ||
                res1->nfs_resop4_u.opillegal.status ==
                NFS4ERR_DENIED) {
                res1->nfs_resop4_u.opillegal.status =
                        NFS4ERR_RETRY_UNCACHED_REP;
            }

            c_res->status = res1->nfs_resop4_u.opillegal.status;
        }

        /* NOTE: We just built a 2nd "uncached" response and put that
         * in the slot cache with 1 reference. The actual response is
         * whatever it is, but is different and has it's OWN 1 refcount.
         * It can't have more than 1 reference since this is NOT a
         * replay.
         */
    }

    /* If we have reserved a lease, update it and release it */
    if (data->preserved_clientid != NULL) {
        /* Update and release lease */
        PTHREAD_MUTEX_lock(&data->preserved_clientid->cid_mutex);

        update_lease(data->preserved_clientid);

        PTHREAD_MUTEX_unlock(&data->preserved_clientid->cid_mutex);
    }

    if (status != NFS4_OK)
        LogDebug(COMPONENT_NFS_V4, "End status = %s lastindex = %d",
                 nfsstat4_to_str(status), data->oppos);

}

static enum xprt_stat nfs4_compound_resume(struct svc_req *req)
{
    nfs_request_t *reqdata = container_of(req, nfs_request_t, svc);
    nfsstat4 status = NFS4_OK;
    compound_data_t *data = reqdata->proc_data;
    enum nfs_req_result result;

    /* Restore the op_ctx */
    resume_op_context(&reqdata->op_context);

    /* Start by resuming the operation that suspended. */
    result = (optabv4[data->opcode].resume)
            (&data->argarray[data->oppos], data, &data->resarray[data->oppos]);

    if (result != NFS_REQ_ASYNC_WAIT) {
        /* Complete the operation (will fill in status). */
        result = complete_op(data, &status, result);
    } else {
        /* The request is suspended, don't touch the request in
         * any way because the resume may already be scheduled
         * and running on nother thread. The xp_resume_cb has
         * already been set up before we started processing
         * ops on this request at all.
         */
        suspend_op_context();
        return XPRT_SUSPEND;
    }

    /* Skip the resumed op and continue through the rest of the compound. */
    for (data->oppos += 1;
         result == NFS_REQ_OK && data->oppos < data->argarray_len;
         data->oppos++) {
        result = process_one_op(data, &status);

        if (result == NFS_REQ_ASYNC_WAIT) {
            /* The request is suspended, don't touch the request in
             * any way because the resume may already be scheduled
             * and running on nother thread. The xp_resume_cb has
             * already been set up before we started processing
             * ops on this request at all.
             */
            suspend_op_context();
            return XPRT_SUSPEND;
        }
    }

    complete_nfs4_compound(data, status, result);

    compound_data_Free(data);
    /* release current active export in op_ctx. */
    if (op_ctx->ctx_export)
        clear_op_context_export();

    nfs_rpc_complete_async_request(reqdata, NFS_REQ_OK);

    return XPRT_IDLE;
}

enum nfs_req_result process_one_op(compound_data_t *data, nfsstat4 *status) {
    const char *bad_op_state_reason = "";
    int perm_flags;

    /* 获取当前处理操作对应的参数和结果数据指针 */
    nfs_argop4 *thisarg = &data->argarray[data->oppos];
    nfs_resop4 *thisres = &data->resarray[data->oppos];
    enum nfs_req_result result;
    COMPOUND4res * res_compound4 = &data->res->res_compound4_extended->res_compound4;

    /* Used to check if OP_SEQUENCE is the first operation */
    data->op_resp_size = sizeof(nfsstat4);
    data->opcode = thisarg->argop;

    /* 确保要处理的操作在指定小版本中是合法的，否则统一作为不合法操作处理 */
    if (data->opcode > LastOpcode[data->minorversion])
        data->opcode = static_cast<nfs_opnum4>(0);

    data->opname = optabv4[data->opcode].name;

    LOG(MODULE_NAME, D_INFO, "Request %d: opcode %d is %s",
        data->oppos, data->opcode, data->opname);

    /* BIND_CONN_TO_SESSION操作只允许单独出现，不能和其他操作组合 */
    if (data->oppos > 0 && data->opcode == NFS4_OP_BIND_CONN_TO_SESSION) {
        *status = NFS4ERR_NOT_ONLY_OP;
        bad_op_state_reason =
                "BIND_CONN_TO_SESSION past position 1";
        goto bad_op_state;
    }

    /* 如果存在OP_SEQUENCE操作，那么他一定处于第一个请求的位置上 */
    if (data->oppos > 0 && data->opcode == NFS4_OP_SEQUENCE) {
        *status = NFS4ERR_SEQUENCE_POS;
        bad_op_state_reason =
                "SEQUENCE past position 1";
        goto bad_op_state;
    }

    /* 如果当前操作是NFS4_OP_DESTROY_SESSION，必须是整个序列操作的最后一个 */
    if (data->oppos > 0 && data->opcode == NFS4_OP_DESTROY_SESSION) {
        bool session_compare;
        bool bad_pos;

        session_compare = memcmp(
                data->argarray[0].nfs_argop4_u.opsequence.sa_sessionid,
                thisarg->nfs_argop4_u.opdestroy_session.dsa_sessionid,
                NFS4_SESSIONID_SIZE) == 0;

        bad_pos = session_compare &&
                  data->oppos != (data->argarray_len - 1);

        LOG(MODULE_NAME, D_INFO,
            "DESTROY_SESSION in position %u out of 0-%"
            PRIi32 " %s is %s",
            data->oppos, data->argarray_len - 1,
            session_compare
            ? "same session as SEQUENCE"
            : "different session from SEQUENCE",
            bad_pos ? "not last op in compound" : "opk");

        if (bad_pos) {
            *status = NFS4ERR_NOT_ONLY_OP;
            bad_op_state_reason =
                    "DESTROY_SESSION not last op in compound";
            goto bad_op_state;
        }
    }

    /* time each op */
    now(&data->op_start_time);

    /* 确保操作数量满足要求，不超过指定的上限 */
    if (data->minorversion > 0 && data->session != NULL &&
        data->session->fore_channel_attrs.ca_maxoperations ==
        data->oppos) {
        *status = NFS4ERR_TOO_MANY_OPS;
        bad_op_state_reason = "Too many operations";
        goto bad_op_state;
    }

    perm_flags = optabv4[data->opcode].exp_perm_flags &
                 EXPORT_OPTION_ACCESS_MASK;

    if (perm_flags != 0) {
        *status = static_cast<nfsstat4>(nfs4_Is_Fh_Empty(&data->currentFH));
        if (*status != NFS4_OK) {
            bad_op_state_reason = "Empty or NULL handle";
            goto bad_op_state;
        }

//        /* Operation uses a CurrentFH, so we can check export
//         * perms. Perms should even be set reasonably for pseudo
//         * file system.
//         */
//        LOG(MODULE_NAME, D_INFO,
//            "Check export perms export = %08x req = %08x",
//            op_ctx->export_perms.options &
//            EXPORT_OPTION_ACCESS_MASK,
//            perm_flags);
//
//        if ((op_ctx->export_perms.options & perm_flags) != perm_flags) {
//            /* Export doesn't allow requested
//             * access for this client.
//             */
//            if ((perm_flags & EXPORT_OPTION_MODIFY_ACCESS)
//                != 0)
//                *status = NFS4ERR_ROFS;
//            else
//                *status = NFS4ERR_ACCESS;
//
//            bad_op_state_reason = "Export permission failure";
//            alt_component = COMPONENT_EXPORT;
//            goto bad_op_state;
//        }
    }

    /* Set up the minimum/default response size and check if there
     * is room for it.
    */
    data->op_resp_size = optabv4[data->opcode].resp_size;

    *status = check_resp_room(data, data->op_resp_size);

    if (*status != NFS4_OK) {
        bad_op_state_reason = "op response size";

        bad_op_state:
        /* Tally the response size */
        data->resp_size += sizeof(nfs_opnum4) + sizeof(nfsstat4);

        LogDebugAlt(COMPONENT_NFS_V4, alt_component,
                    "Status of %s in position %d due to %s is %s, op response size = %"
                    PRIu32" total response size = %"PRIu32,
                    data->opname, data->oppos,
                    bad_op_state_reason,
                    nfsstat4_to_str(*status),
                    data->op_resp_size, data->resp_size);

        /* All the operation, like NFS4_OP_ACCESS, have
         * a first replied field called .status
         */
        thisres->nfs_resop4_u.opaccess.status = *status;
        thisres->resop = data->opcode;

        /* Do not manage the other requests in the COMPOUND. */
        res_compound4->resarray.resarray_len = data->oppos + 1;
        return NFS_REQ_ERROR;
    }

    /***************************************************************
     * Make the actual op call                                     *
     **************************************************************/

    result = (optabv4[data->opcode].funct) (thisarg, data, thisres);

    if (result != NFS_REQ_ASYNC_WAIT) {
        /* Complete the operation, otherwise return without doing
         * anything else.
         */
        result = complete_op(data, status, result);
    }

    return result;
}

int nfs4_compound(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    nfsstat4 status = NFS4_OK;
    compound_data_t *data = NULL;
    const uint32_t compound4_minor = arg->arg_compound4.minorversion;
    const uint32_t argarray_len = arg->arg_compound4.argarray.argarray_len;
    /* Array of op arguments */
    nfs_argop4 * const argarray = arg->arg_compound4.argarray.argarray_val;

    bool drop = false;
    nfs_request_t *reqdata = get_parent_struct_addr(req, nfs_request_t, svc);
    enum nfs_req_result result = NFS_REQ_OK;

    /* 分配返回结果的数据空间，并对引用进行显式计数 */
    res->res_compound4_extended = reinterpret_cast<COMPOUND4res_extended*>(
            calloc(1, sizeof(*res->res_compound4_extended)));
    struct COMPOUND4res *res_compound4 = &res->res_compound4_extended->res_compound4;
    res->res_compound4_extended->res_refcnt = 1;

    /* 版本检查确保小版本不得超过最大的小版本 */
    if (compound4_minor > NFS_V4_MAX_MINOR) {
        LOG(MODULE_NAME, L_ERROR, "Bad Minor Version %d(Max:2)", compound4_minor);
        res_compound4->status = NFS4ERR_MINOR_VERS_MISMATCH;
        res_compound4->resarray.resarray_len = 0;
        goto out;
    }

    /* 确保小版本是版本支持bitmap中标记支持的 */
    if ((nfs_param.nfsv4_param.minor_versions &
         (1 << compound4_minor)) == 0) {
        LOG(MODULE_NAME, L_ERROR, "Unsupported minor version %d",
            compound4_minor);
        res_compound4->status = NFS4ERR_MINOR_VERS_MISMATCH;
        res_compound4->resarray.resarray_len = 0;
        goto out;
    }

    /* 初始化内部的compound处理结果数据结构体 */
    data = reinterpret_cast<compound_data*>(
            calloc(1, sizeof(*data)));
    data->req = req;
    data->argarray_len = argarray_len;
    data->argarray = arg->arg_compound4.argarray.argarray_val;
    data->res = res;
    data->minorversion = compound4_minor;

    reqdata->proc_data = data;

    /* 保持输入tag和输出tag的一致性 */
    copy_tag(&res_compound4->tag, &arg->arg_compound4.tag);
    res_compound4->tag.utf8string_len =
            arg->arg_compound4.tag.utf8string_len;

    /* 检查输入tag的合法性 */
    if (res_compound4->tag.utf8string_len > 0) {
        /* Check if the tag is a valid utf8 string (., .., and / ok) */
        if (nfs4_utf8string_scan(&res_compound4->tag,
                                 UTF8_SCAN_STRICT) != 0) {

            LOG(MODULE_NAME, L_ERROR, "COMPOUND: bad tag %p len %d",
                res_compound4->tag.utf8string_val,
                res_compound4->tag.utf8string_len);

            res_compound4->status = NFS4ERR_INVAL;
            res_compound4->resarray.resarray_len = 0;
            goto out;
        }

        /* Make a copy of the tagname */
        data->tagname = reinterpret_cast<char*>(
                malloc(res_compound4->tag.utf8string_len + 1));
        memcpy(data->tagname,
               res_compound4->tag.utf8string_val,
               res_compound4->tag.utf8string_len + 1);
    } else {
        /* No tag */
        data->tagname = strdup("NO TAG");
    }

    LOG(MODULE_NAME, D_INFO,
        "COMPOUND: There are %d operations, res = %p, tag = %s",
        argarray_len, res, data->tagname);

    /* 请求遇到了空操作请求 */
    if (argarray_len == 0) {
        LOG(MODULE_NAME, L_INFO,
            "An empty COMPOUND (no operation in it) was received");

        res_compound4->status = NFS4_OK;
        res_compound4->resarray.resarray_len = 0;
        goto out;
    }

    /* 单次请求包含的操作个数不能超过NFS4_MAX_OPERATIONS */
    if (argarray_len > NFS4_MAX_OPERATIONS) {
        LOG(MODULE_NAME, L_INFO,
            "A COMPOUND with too many operations (%d) was received",
            argarray_len);

        res_compound4->status = NFS4ERR_RESOURCE;
        res_compound4->resarray.resarray_len = 0;
        goto out;
    }

    /* 初始化返回数据大小，返回数据中的数据实体指针会被替换为数据，因此不会计算指针占用的空间 */
    data->resp_size = sizeof(COMPOUND4res) - sizeof(nfs_resop4 *);

    /* 从请求数据中构建客户端认证信息数据 */
    if (nfs_rpc_req2client_cred(req, &data->credential) == -1) {
        /* Malformed credential */
        drop = true;
        goto out;
    }

    /* 根据输入请求的个数分配结果结构体对应的空间 */
    data->resarray = reinterpret_cast<nfs_resop4*>(
            calloc(argarray_len, sizeof(struct nfs_resop4)));
    res_compound4->resarray.resarray_len = argarray_len;
    res_compound4->resarray.resarray_val = data->resarray;

    /* 针对4.1中的两种特殊场景进行过滤处理，确保op组合的合理性 */
    if (compound4_minor > 0) {
        /* Check for valid operation to start an NFS v4.1 COMPOUND */
        if (argarray[0].argop != NFS4_OP_ILLEGAL
            && argarray[0].argop != NFS4_OP_SEQUENCE
            && argarray[0].argop != NFS4_OP_EXCHANGE_ID
            && argarray[0].argop != NFS4_OP_CREATE_SESSION
            && argarray[0].argop != NFS4_OP_DESTROY_SESSION
            && argarray[0].argop != NFS4_OP_BIND_CONN_TO_SESSION
            && argarray[0].argop != NFS4_OP_DESTROY_CLIENTID) {
            res_compound4->status = NFS4ERR_OP_NOT_IN_SESSION;
            res_compound4->resarray.resarray_len = 0;
            goto out;
        }

        if (argarray_len > 1) {
            /* If not prepended by OP4_SEQUENCE, OP4_EXCHANGE_ID
             * should be the only request in the compound see
             * 18.35.3. and test EID8 for details
             *
             * If not prepended bu OP4_SEQUENCE, OP4_CREATE_SESSION
             * should be the only request in the compound see
             * 18.36.3 and test CSESS23 for details
             *
             * If the COMPOUND request does not start with SEQUENCE,
             * and if DESTROY_SESSION is not the sole operation,
             * then server MUST return  NFS4ERR_NOT_ONLY_OP. See
             * 18.37.3 and test DSESS9005 for details
             */
            if (argarray[0].argop == NFS4_OP_EXCHANGE_ID ||
                argarray[0].argop == NFS4_OP_CREATE_SESSION ||
                argarray[0].argop == NFS4_OP_DESTROY_CLIENTID ||
                argarray[0].argop == NFS4_OP_DESTROY_SESSION ||
                argarray[0].argop == NFS4_OP_BIND_CONN_TO_SESSION) {
                res_compound4->status = NFS4ERR_NOT_ONLY_OP;
                res_compound4->resarray.resarray_len = 0;
                goto out;
            }
        }
    }

    /* 打印本次请求组合操作的debug详细信息，打印相关的操作名字 */
    if (unlikely(logger.is_module_debug_on(MODULE_NAME))) {
        nfs_opnum4 opcodes[NFS4_MAX_OPERATIONS] = {static_cast<nfs_opnum4>(0)};
        get_nfs4_opcodes(data, opcodes, NFS4_MAX_OPERATIONS);

        string opcodes_str;
        for (auto& opcode : opcodes) {
            opcodes_str += nfsop4_to_str(opcode);
        }

        LOG(MODULE_NAME, D_INFO,
            "COMPOUND: There are %d operations %s",
            argarray_len, opcodes_str.c_str());
    }

    /* Before we start running, we must prepare to be suspended. We must do
     * this now because after we have been suspended, it's too late, the
     * request might have already been resumed on another worker thread.
     */
    data->req->rq_resume_cb = nfs4_compound_resume;

    /**********************************************************************
     * 正式开始执行具体的微操作处理任务，按照输入操作的顺序逐个响应请求
     **********************************************************************/
    for (data->oppos = 0;
         result == NFS_REQ_OK && data->oppos < data->argarray_len;
         data->oppos++) {
        result = process_one_op(data, &status);

        if (result == NFS_REQ_ASYNC_WAIT) {
            /* 如果一个操作被异步处理，那么这里会立即返回，并且 */
            /* The request is suspended, don't touch the request in
             * any way because the resume may already be scheduled
             * and running on nother thread. The xp_resume_cb has
             * already been set up before we started processing
             * ops on this request at all.
             */
            return result;
        }
    }

    complete_nfs4_compound(data, status, result);

    out:

    compound_data_Free(data);

    return drop ? NFS_REQ_DROP : NFS_REQ_OK;
}

/**
 *
 * @brief Free the result for one NFS4_OP
 *
 * This function frees any memory allocated for the result of an NFSv4
 * operation.
 *
 * @param[in,out] res The result to be freed
 *
 */
void nfs4_Compound_FreeOne(nfs_resop4 *res)
{
    int opcode;

    opcode = (res->resop != NFS4_OP_ILLEGAL)
             ? res->resop : 0;	/* opcode 0 for illegals */
    optabv4[opcode].free_res(res);
}

void release_nfs4_res_compound(struct COMPOUND4res_extended *res_compound4_ex)
{
    unsigned int i = 0;
    int32_t refcnt = atomic_dec_int32_t(&res_compound4_ex->res_refcnt);
    struct COMPOUND4res *res_compound4 = &res_compound4_ex->res_compound4;

    assert(refcnt >= 0);

    if (refcnt > 0) {
        LogFullDebugAlt(COMPONENT_NFS_V4, COMPONENT_SESSIONS,
                        "Skipping free of NFS4 result %p refcnt %"PRIi32,
                        res_compound4_ex, refcnt);
        return;
    }

    LogFullDebugAlt(COMPONENT_NFS_V4, COMPONENT_SESSIONS,
                    "Compound Free %p (resarraylen=%i)",
                    res_compound4_ex, res_compound4->resarray.resarray_len);

    for (i = 0; i < res_compound4->resarray.resarray_len; i++) {
        nfs_resop4 *val = &res_compound4->resarray.resarray_val[i];

        if (val) {
            /* !val is an error case, but it can occur, so avoid
             * indirect on NULL
             */
            nfs4_Compound_FreeOne(val);
        }
    }

    gsh_free(res_compound4->resarray.resarray_val);
    res_compound4->resarray.resarray_val = NULL;

    gsh_free(res_compound4->tag.utf8string_val);
    res_compound4->tag.utf8string_val = NULL;

    gsh_free(res_compound4_ex);
}

/**
 *
 * @brief Free the result for NFS4PROC_COMPOUND
 *
 * This function frees the result for one NFS4PROC_COMPOUND.
 *
 * @param[in] res The result
 *
 */
void nfs4_compound_free(nfs_res_t *res)
{
    release_nfs4_res_compound(res->res_compound4_extended);
}

/**
 *
 * @brief Copy the result for one NFS4_OP
 *
 * This function copies the result structure for a single NFSv4
 * operation.
 *
 * @param[out] res_dst Buffer to which to copy the result
 * @param[in]  res_src The result to copy
 *
 */
void nfs4_Compound_CopyResOne(nfs_resop4 *res_dst, nfs_resop4 *res_src)
{
    /* Copy base data structure */
    memcpy(res_dst, res_src, sizeof(*res_dst));

    /* Do deep copy where necessary */
    switch (res_src->resop) {
        case NFS4_OP_ACCESS:
            break;

        case NFS4_OP_CLOSE:
            nfs4_op_close_CopyRes(&res_dst->nfs_resop4_u.opclose,
                                  &res_src->nfs_resop4_u.opclose);
            return;

        case NFS4_OP_COMMIT:
        case NFS4_OP_CREATE:
        case NFS4_OP_DELEGPURGE:
        case NFS4_OP_DELEGRETURN:
        case NFS4_OP_GETATTR:
        case NFS4_OP_GETFH:
        case NFS4_OP_LINK:
            break;

        case NFS4_OP_LOCK:
            nfs4_op_lock_CopyRes(&res_dst->nfs_resop4_u.oplock,
                                 &res_src->nfs_resop4_u.oplock);
            return;

        case NFS4_OP_LOCKT:
            break;

        case NFS4_OP_LOCKU:
            nfs4_op_locku_CopyRes(&res_dst->nfs_resop4_u.oplocku,
                                  &res_src->nfs_resop4_u.oplocku);
            return;

        case NFS4_OP_LOOKUP:
        case NFS4_OP_LOOKUPP:
        case NFS4_OP_NVERIFY:
            break;

        case NFS4_OP_OPEN:
            nfs4_op_open_CopyRes(&res_dst->nfs_resop4_u.opopen,
                                 &res_src->nfs_resop4_u.opopen);
            return;

        case NFS4_OP_OPENATTR:
            break;

        case NFS4_OP_OPEN_CONFIRM:
            nfs4_op_open_confirm_CopyRes(
                    &res_dst->nfs_resop4_u.opopen_confirm,
                    &res_src->nfs_resop4_u.opopen_confirm);
            return;

        case NFS4_OP_OPEN_DOWNGRADE:
            nfs4_op_open_downgrade_CopyRes(
                    &res_dst->nfs_resop4_u.opopen_downgrade,
                    &res_src->nfs_resop4_u.opopen_downgrade);
            return;

        case NFS4_OP_PUTFH:
        case NFS4_OP_PUTPUBFH:
        case NFS4_OP_PUTROOTFH:
        case NFS4_OP_READ:
        case NFS4_OP_READDIR:
        case NFS4_OP_READLINK:
        case NFS4_OP_REMOVE:
        case NFS4_OP_RENAME:
        case NFS4_OP_RENEW:
        case NFS4_OP_RESTOREFH:
        case NFS4_OP_SAVEFH:
        case NFS4_OP_SECINFO:
        case NFS4_OP_SETATTR:
        case NFS4_OP_SETCLIENTID:
        case NFS4_OP_SETCLIENTID_CONFIRM:
        case NFS4_OP_VERIFY:
        case NFS4_OP_WRITE:
        case NFS4_OP_RELEASE_LOCKOWNER:
            break;

        case NFS4_OP_EXCHANGE_ID:
        case NFS4_OP_CREATE_SESSION:
        case NFS4_OP_SEQUENCE:
        case NFS4_OP_GETDEVICEINFO:
        case NFS4_OP_GETDEVICELIST:
        case NFS4_OP_BACKCHANNEL_CTL:
        case NFS4_OP_BIND_CONN_TO_SESSION:
        case NFS4_OP_DESTROY_SESSION:
        case NFS4_OP_FREE_STATEID:
        case NFS4_OP_GET_DIR_DELEGATION:
        case NFS4_OP_LAYOUTCOMMIT:
        case NFS4_OP_LAYOUTGET:
        case NFS4_OP_LAYOUTRETURN:
        case NFS4_OP_SECINFO_NO_NAME:
        case NFS4_OP_SET_SSV:
        case NFS4_OP_TEST_STATEID:
        case NFS4_OP_WANT_DELEGATION:
        case NFS4_OP_DESTROY_CLIENTID:
        case NFS4_OP_RECLAIM_COMPLETE:

            /* NFSv4.2 */
        case NFS4_OP_ALLOCATE:
        case NFS4_OP_COPY:
        case NFS4_OP_COPY_NOTIFY:
        case NFS4_OP_DEALLOCATE:
        case NFS4_OP_IO_ADVISE:
        case NFS4_OP_LAYOUTERROR:
        case NFS4_OP_LAYOUTSTATS:
        case NFS4_OP_OFFLOAD_CANCEL:
        case NFS4_OP_OFFLOAD_STATUS:
        case NFS4_OP_READ_PLUS:
        case NFS4_OP_SEEK:
        case NFS4_OP_WRITE_SAME:
        case NFS4_OP_CLONE:

            /* NFSv4.3 */
        case NFS4_OP_GETXATTR:
        case NFS4_OP_SETXATTR:
        case NFS4_OP_LISTXATTR:
        case NFS4_OP_REMOVEXATTR:

        case NFS4_OP_LAST_ONE:
            break;

        case NFS4_OP_ILLEGAL:
            break;
    }			/* switch */

    LogFatal(COMPONENT_NFS_V4,
             "Copy one result not implemented for %d",
             res_src->resop);
}

/**
 * @brief Handle the xdr encode of the COMPOUND response
 *
 * @param(in) xdrs  The XDR object
 * @param(in) objp  The response pointer
 *
 */

bool xdr_COMPOUND4res_extended(XDR *xdrs, struct COMPOUND4res_extended **objp)
{
    /* Since the response in nfs_res_t is a pointer, we must dereference it
     * to complete the encode.
     */
    struct COMPOUND4res_extended *res_compound4_extended = *objp;

    /* And we must pass the actual COMPOUND4res */
    return xdr_COMPOUND4res(xdrs, &res_compound4_extended->res_compound4);
}
