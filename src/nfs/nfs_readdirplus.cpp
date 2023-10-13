/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *              Piyuyang pi_yuyang@163.com
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

#include "nfs/nfs_readdirplus.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "dnfsd/dnfs_config.h"
#include "string"

using namespace std;

#define MODULE_NAME "NFS"

int nfs3_readdirplus(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    uint64_t mem_avail = 0;
    // response size
    uint32_t maxcount;
    uint32_t cfg_readdir_size = nfs_param.core_param.readdir_res_size;
    // max entries count
    uint32_t dircount;
    uint32_t usecount = 0;
    uint32_t entry_size = sizeof(entryplus3);
    uint32_t cfg_readdir_count = nfs_param.core_param.readdir_max_count;
    int rc = NFS_REQ_OK;
    /*数据指针*/
    READDIRPLUS3args *readdirplus_args = &arg->arg_readdirplus3;
    READDIRPLUS3resfail *readdirplus_res_fail =
            &res->res_readdirplus3.READDIRPLUS3res_u.resfail;
    READDIRPLUS3resok *readdirplus_res_ok =
            &res->res_readdirplus3.READDIRPLUS3res_u.resok;

    readdirplus_res_ok->reply.entries = nullptr;

    uint64_t begin_cookie = readdirplus_args->cookie;
    entryplus3 *head;
    entryplus3 *current;
    entryplus3 *node;
    // scandir相关
    struct dirent **namelist;
    int n;
    int index = 0;

    string file_path;

    if (readdirplus_args->dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "nfs_readdirplus get file handle len is 0");
        goto out;
    }

    get_file_handle(readdirplus_args->dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_readdirplus obtained dir handle is '%s', and the length is '%d'",
        readdirplus_args->dir.data.data_val,
        readdirplus_args->dir.data.data_len);

    // arg_readdirplus3.maxcount 返回结构体READDIRPLUS3resok最大大小
    if (cfg_readdir_size < readdirplus_args->maxcount)
        maxcount = cfg_readdir_size;
    else
        maxcount = readdirplus_args->maxcount;
    mem_avail = maxcount - BYTES_PER_XDR_UNIT - BYTES_PER_XDR_UNIT - sizeof(fattr3) -
                sizeof(cookieverf3);

    // arg_readdirplus3.dircount 返回的目录信息的最大大小
    if (readdirplus_args->dircount < cfg_readdir_count)
        dircount = readdirplus_args->dircount;
    else
        dircount = cfg_readdir_count;

    /* to avoid setting it on each error case */
    readdirplus_res_fail->dir_attributes.attributes_follow = FALSE;


    res->res_readdirplus3.status = nfs_set_post_op_attr(
            readdirplus_args->dir.data.data_val,
            &res->res_readdirplus3.READDIRPLUS3res_u.resok.dir_attributes);
    if (res->res_readdirplus3.status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR, "nfs_readdirplus stat '%s' failed",
            readdirplus_args->dir.data.data_val);
        rc = NFS_REQ_ERROR;
        goto out;
    }
    if (readdirplus_res_ok->dir_attributes.post_op_attr_u.attributes.type != NF3DIR) {
        LOG(MODULE_NAME, D_ERROR, "nfs_readdirplus get handle type is '%s', not dir",
            readdirplus_res_ok->dir_attributes.post_op_attr_u.attributes.type);
        res->res_readdirplus3.status = NFS3ERR_NOTDIR;
        rc = NFS_REQ_OK;
        goto out;
    }

    n = scandir(readdirplus_args->dir.data.data_val, &namelist, nullptr, alphasort);
    if (n < 0) {
        LOG(MODULE_NAME, D_ERROR, "nfs_readdirplus scandir '%s' failed",
            readdirplus_args->dir.data.data_val);
        res->res_readdirplus3.status = NFS3ERR_BADHANDLE;
        rc = NFS_REQ_ERROR;
        goto out;
    } else {
        head = new entryplus3;
        current = head;
        // todo 分页
        // while (usecount + entry_size < dircount)
        while (index < n) {
            LOG(MODULE_NAME, D_INFO, "nfs_readdirplus get '%s' entry: %s",
                readdirplus_args->dir.data.data_val, namelist[index]->d_name);
            node = new entryplus3;
            node->name = namelist[index]->d_name;
            node->fileid = namelist[index]->d_ino;
            node->nextentry = nullptr;
            if (strcmp(node->name, ".") == 0 || strcmp(node->name, "..") == 0) {
                node->name_attributes.attributes_follow = FALSE;
                node->name_handle.handle_follows = FALSE;
            } else {
                file_path =
                        string(readdirplus_args->dir.data.data_val) + "/" + node->name;
                set_file_handle(&node->name_handle.post_op_fh3_u.handle, file_path);
                node->name_handle.handle_follows = TRUE;
                if (nfs_set_post_op_attr(
                        node->name_handle.post_op_fh3_u.handle.data.data_val,
                        &node->name_attributes) != NFS3_OK) {
                    rc = NFS_REQ_ERROR;
                    LOG(MODULE_NAME, D_ERROR, "nfs_readdirplus 'stat %s' failed",
                        node->name_handle.post_op_fh3_u.handle.data.data_val);
                    goto out;
                }
            }
            usecount += entry_size;
            index++;
            current->nextentry = node;
            current = current->nextentry;
        }
        readdirplus_res_ok->reply.entries = head->nextentry;
    }
    readdirplus_res_ok->reply.eof = TRUE;

    out:
    return rc;
}

void nfs3_readdirplus_free(nfs_res_t *res) {
    /* Nothing to do here */
    // delete entry
    entryplus3 *cur = res->res_readdirplus3.READDIRPLUS3res_u.resok.reply.entries;
    entryplus3 *nxt;
    LOG(MODULE_NAME,L_INFO,"readdirplus free");
    while (cur != nullptr and cur->nextentry != nullptr) {
        nxt = cur->nextentry;
        if (cur->name_handle.handle_follows) {
            free(cur->name_handle.post_op_fh3_u.handle.data.data_val);
        }
        delete (cur);
        cur = nxt;
    }
}

bool xdr_READDIRPLUS3args(XDR *xdrs, READDIRPLUS3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->dir))
        return FALSE;
    if (!xdr_cookie3(xdrs, &objp->cookie))
        return FALSE;
    if (!xdr_cookieverf3(xdrs, objp->cookieverf))
        return FALSE;
    if (!xdr_count3(xdrs, &objp->dircount))
        return FALSE;
    if (!xdr_count3(xdrs, &objp->maxcount))
        return FALSE;
    return TRUE;
}

bool xdr_entryplus3(XDR *xdrs, entryplus3 *objp) {
    if (!xdr_fileid3(xdrs, &objp->fileid))
        return FALSE;
    if (!xdr_filename3(xdrs, &objp->name))
        return FALSE;
    if (!xdr_cookie3(xdrs, &objp->cookie))
        return FALSE;
    if (!xdr_post_op_attr(xdrs, &objp->name_attributes))
        return FALSE;
    if (!xdr_post_op_fh3(xdrs, &objp->name_handle))
        return FALSE;
    if (!xdr_pointer(xdrs, (void **) &objp->nextentry, sizeof(entryplus3),
                     (xdrproc_t) xdr_entryplus3))
        return FALSE;
    return TRUE;
}

bool xdr_dirlistplus3(XDR *xdrs, dirlistplus3 *objp) {
    if (!xdr_pointer(xdrs, (void **) &objp->entries, sizeof(entryplus3),
                     (xdrproc_t) xdr_entryplus3))
        return FALSE;
    if (!xdr_bool(xdrs, &objp->eof))
        return FALSE;
    return TRUE;
}

bool xdr_READDIRPLUS3resok(XDR *xdrs, READDIRPLUS3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->dir_attributes))
        return FALSE;
    if (!xdr_cookieverf3(xdrs, objp->cookieverf))
        return FALSE;
    if (!xdr_dirlistplus3(xdrs, &objp->reply))
        return FALSE;
    return TRUE;
}

bool xdr_READDIRPLUS3resfail(XDR *xdrs, READDIRPLUS3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->dir_attributes))
        return FALSE;
    return TRUE;
}

bool xdr_READDIRPLUS3res(XDR *xdrs, READDIRPLUS3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_READDIRPLUS3resok(xdrs, &objp->READDIRPLUS3res_u.resok))
                return FALSE;
            break;
        default:
            if (!xdr_READDIRPLUS3resfail(xdrs, &objp->READDIRPLUS3res_u.resfail))
                return FALSE;
            break;
    }
    return TRUE;
}