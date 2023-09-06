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

#include "dnfsd/dnfs_config.h"
#include "dnfsd/dnfs_meta_data.h"
#include "nfs/nfs_base.h"
#include "nfs/nfs_utils.h"
#include "nfs/nfs_compound_base.h"
#include "utils/common_utils.h"
#include "log/log.h"

#define MODULE_NAME "DNFS"

int nfs4_compound(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    nfsstat4 status = NFS4_OK;
    compound_data_t *data = NULL;
    const uint32_t compound4_minor = arg->arg_compound4.minorversion;
    const uint32_t argarray_len = arg->arg_compound4.argarray.argarray_len;
    /* Array of op arguments */
    nfs_argop4 * const argarray = arg->arg_compound4.argarray.argarray_val;
    bool drop = false;
    nfs_request_t *reqdata = get_parent_struct_addr(req, nfs_request_t, svc);
    struct COMPOUND4res *res_compound4;
    enum nfs_req_result result = NFS_REQ_OK;

    /* Allocate (and zero) the COMPOUND4res_extended */
    res->res_compound4_extended = reinterpret_cast<COMPOUND4res_extended*>(
            calloc(1, sizeof(*res->res_compound4_extended)));
    res_compound4 = &res->res_compound4_extended->res_compound4;

    /* Take initial reference to response. */
    res->res_compound4_extended->res_refcnt = 1;

    if (compound4_minor > 2) {
        LOG(MODULE_NAME, L_ERROR, "Bad Minor Version %d", compound4_minor);

        res_compound4->status = NFS4ERR_MINOR_VERS_MISMATCH;
        res_compound4->resarray.resarray_len = 0;
        goto out;
    }

    if ((nfs_param.nfsv4_param.minor_versions &
         (1 << compound4_minor)) == 0) {
        LOG(MODULE_NAME, L_ERROR, "Unsupported minor version %d",
            compound4_minor);
        res_compound4->status = NFS4ERR_MINOR_VERS_MISMATCH;
        res_compound4->resarray.resarray_len = 0;
        goto out;
    }

    /* Initialisation of the compound request internal's data */
    data = reinterpret_cast<compound_data*>(
            calloc(1, sizeof(*data)));

    data->req = req;
    data->argarray_len = argarray_len;
    data->argarray = arg->arg_compound4.argarray.argarray_val;
    data->res = res;
    reqdata->proc_data = data;

    /* Minor version related stuff */
    data->minorversion = compound4_minor;

    /* Keeping the same tag as in the arguments */
    copy_tag(&res_compound4->tag, &arg->arg_compound4.tag);

    if (res_compound4->tag.utf8string_len > 0) {
        /* Check if the tag is a valid utf8 string (., .., and / ok) */
        if (nfs4_utf8string_scan(&res_compound4->tag,
                                 UTF8_SCAN_STRICT) != 0) {
            char str[LOG_BUFF_LEN];
            struct display_buffer dspbuf = {sizeof(str), str, str};

            display_opaque_bytes(
                    &dspbuf,
                    res_compound4->tag.utf8string_val,
                    res_compound4->tag.utf8string_len);

            LogCrit(COMPONENT_NFS_V4,
                    "COMPOUND: bad tag %p len %d bytes %s",
                    res_compound4->tag.utf8string_val,
                    res_compound4->tag.utf8string_len,
                    str);

            res_compound4->status = NFS4ERR_INVAL;
            res_compound4->resarray.resarray_len = 0;
            goto out;
        }

        /* Make a copy of the tagname */
        data->tagname =
                gsh_malloc(res_compound4->tag.utf8string_len + 1);
        memcpy(data->tagname,
               res_compound4->tag.utf8string_val,
               res_compound4->tag.utf8string_len + 1);
    } else {
        /* No tag */
        data->tagname = gsh_strdup("NO TAG");
    }

    /* Managing the operation list */
    LogDebug(COMPONENT_NFS_V4,
             "COMPOUND: There are %d operations, res = %p, tag = %s",
             argarray_len, res, data->tagname);

    /* Check for empty COMPOUND request */
    if (argarray_len == 0) {
        LogMajor(COMPONENT_NFS_V4,
                 "An empty COMPOUND (no operation in it) was received");

        res_compound4->status = NFS4_OK;
        res_compound4->resarray.resarray_len = 0;
        goto out;
    }

    /* Check for too long request */
    if (argarray_len > NFS4_MAX_OPERATIONS) {
        LogMajor(COMPONENT_NFS_V4,
                 "A COMPOUND with too many operations (%d) was received",
                 argarray_len);

        res_compound4->status = NFS4ERR_RESOURCE;
        res_compound4->resarray.resarray_len = 0;
        goto out;
    }

    /* Initialize response size with size of compound response size. */
    data->resp_size = sizeof(COMPOUND4res) - sizeof(nfs_resop4 *);

    /* Building the client credential field */
    if (nfs_rpc_req2client_cred(req, &data->credential) == -1) {
        /* Malformed credential */
        drop = true;
        goto out;
    }

    /* Keeping the same tag as in the arguments */
    res_compound4->tag.utf8string_len =
            arg->arg_compound4.tag.utf8string_len;

    /* Allocating the reply nfs_resop4 */
    data->resarray = gsh_calloc(argarray_len, sizeof(struct nfs_resop4));

    res_compound4->resarray.resarray_len = argarray_len;
    res_compound4->resarray.resarray_val = data->resarray;

    /* Manage errors NFS4ERR_OP_NOT_IN_SESSION and NFS4ERR_NOT_ONLY_OP.
     * These checks apply only to 4.1 */
    if (compound4_minor > 0) {
        /* Check for valid operation to start an NFS v4.1 COMPOUND:
         */
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

    if (likely(component_log_level[COMPONENT_NFS_V4] >= NIV_FULL_DEBUG)) {
        nfs_opnum4 opcodes[NFS4_MAX_OPERATIONS] = {0};
        uint32_t opcode_num = get_nfs4_opcodes(data,
                                               opcodes, NFS4_MAX_OPERATIONS);

        char operations[NFS4_COMPOUND_OPERATIONS_STR_LEN] = "\0";
        struct display_buffer dspbuf = {sizeof(operations),
                                        operations, operations};

        display_nfs4_operations(&dspbuf, opcodes, opcode_num);

        LogFullDebug(COMPONENT_NFS_V4,
                     "COMPOUND: There are %d operations %s",
                     argarray_len, operations);
    }

    /* Before we start running, we must prepare to be suspended. We must do
     * this now because after we have been suspended, it's too late, the
     * request might have already been resumed on another worker thread.
     */
    data->req->rq_resume_cb = nfs4_compound_resume;

    /**********************************************************************
     * Now start processing the compound ops.
     **********************************************************************/
    for (data->oppos = 0;
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
            return result;
        }
    }

    complete_nfs4_compound(data, status, result);

    out:

    compound_data_Free(data);

    /* release current active export in op_ctx. */
    if (op_ctx->ctx_export)
        clear_op_context_export();

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
}				/* compound_data_Free */

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
