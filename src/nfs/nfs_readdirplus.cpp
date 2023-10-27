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

int nfs3_readdirplus(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    timespec ctime;
    uint64_t change;
    cookieverf3 cookie_verifier;
    uint64_t mem_avail = 0;
    uint64_t used_mem = 0;
    // response size
    uint32_t max_res_size;
    uint32_t cfg_readdir_size = nfs_param.core_param.readdir_res_size;
    // max entries count
    uint32_t max_entry_count;
    uint32_t cfg_readdir_count = nfs_param.core_param.readdir_max_count;
    uint32_t entry_count = 0;
    int rc = NFS_REQ_OK;
    /*数据指针*/
    READDIRPLUS3args *readdirplus_args = &arg->arg_readdirplus3;
    READDIRPLUS3resfail *readdirplus_res_fail =
        &res->res_readdirplus3.READDIRPLUS3res_u.resfail;
    READDIRPLUS3resok *readdirplus_res_ok =
        &res->res_readdirplus3.READDIRPLUS3res_u.resok;

    readdirplus_res_ok->reply.entries = nullptr;
    readdirplus_res_ok->reply.eof = FALSE;

    uint64_t begin_cookie = readdirplus_args->cookie;
    uint64_t cookie = 0;
    int dir_fh;
    off_t seekloc = 0;
    off_t baseloc = 0;
    uint32_t bpos;
    int nread;
    vfs_dirent dentry = {}, *dentryp = &dentry;
    char buf[BUF_SIZE];
    entryplus3 *head;
    entryplus3 *current;
    entryplus3 *node;
    string file_path;

    if (readdirplus_args->dir.data.data_len == 0)
    {
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
        max_res_size = cfg_readdir_size;
    else
        max_res_size = readdirplus_args->maxcount;
    mem_avail = MIN(WRITE_READ_MAX,
                    max_res_size - BYTES_PER_XDR_UNIT - BYTES_PER_XDR_UNIT - sizeof(fattr3) - sizeof(cookieverf3));

    // arg_readdirplus3.dircount 返回的目录信息的最大大小
    if (readdirplus_args->dircount < cfg_readdir_count)
        max_entry_count = readdirplus_args->dircount;
    else
        max_entry_count = cfg_readdir_count;

    LOG(MODULE_NAME, D_INFO,
        "NFS3_READDIRPLUS: dircount=%u begin_cookie=%u mem_avail=%zd max_count = %u",
        arg->arg_readdirplus3.dircount, begin_cookie, mem_avail, max_entry_count);

    /* to avoid setting it on each error case */
    readdirplus_res_fail->dir_attributes.attributes_follow = FALSE;

    res->res_readdirplus3.status = nfs_set_post_op_attr(
        readdirplus_args->dir.data.data_val,
        &res->res_readdirplus3.READDIRPLUS3res_u.resok.dir_attributes);
    if (res->res_readdirplus3.status != NFS3_OK)
    {
        LOG(MODULE_NAME, D_ERROR, "nfs_readdirplus stat '%s' failed",
            readdirplus_args->dir.data.data_val);
        rc = NFS_REQ_ERROR;
        goto out;
    }
    if (readdirplus_res_ok->dir_attributes.post_op_attr_u.attributes.type != NF3DIR)
    {
        LOG(MODULE_NAME, D_ERROR, "nfs_readdirplus get handle type is '%s', not dir",
            readdirplus_res_ok->dir_attributes.post_op_attr_u.attributes.type);
        res->res_readdirplus3.status = NFS3ERR_NOTDIR;
        rc = NFS_REQ_OK;
        goto out;
    }

    memset(cookie_verifier, 0, sizeof(cookie_verifier));
    ctime.tv_nsec = readdirplus_res_ok->dir_attributes.post_op_attr_u.attributes.ctime.tv_nsec;
    ctime.tv_sec = readdirplus_res_ok->dir_attributes.post_op_attr_u.attributes.ctime.tv_sec;
    change = timespec_to_nsecs(&ctime);
    memcpy(cookie_verifier, &change, MIN(sizeof(cookie_verifier), sizeof(change)));

    if (begin_cookie != 0)
    {
        /* Not the first call, so we have to check the cookie
           verifier */
        if (memcmp(cookie_verifier,
                   arg->arg_readdirplus3.cookieverf,
                   NFS3_COOKIEVERFSIZE) != 0)
        {
            res->res_readdirplus3.status = NFS3ERR_BAD_COOKIE;
            rc = NFS_REQ_OK;
            goto out;
        }
    }

    /* Fudge cookie for "." and "..", if necessary */
    if (begin_cookie > 2)
        cookie = begin_cookie;
    else
        cookie = 0;

    dir_fh = open(readdirplus_args->dir.data.data_val, O_RDONLY | O_DIRECTORY);
    if (dir_fh < 0)
    {
        res->res_readdirplus3.status = NFS3ERR_BADHANDLE;
        rc = NFS_REQ_ERROR;
        goto done;
    }
    seekloc = (off_t)cookie;
    seekloc = lseek(dir_fh, seekloc, SEEK_SET);
    if (seekloc < 0)
    {
        res->res_readdirplus3.status = NFS3ERR_BADHANDLE;
        rc = NFS_REQ_ERROR;
        goto done;
    }
    head = new entryplus3;
    head->nextentry = nullptr;
    current = head;
    do
    {
        baseloc = seekloc;
        nread = vfs_readents(dir_fh, buf, BUF_SIZE, &seekloc);
        if (nread < 0)
        {
            res->res_readdirplus3.status = NFS3ERR_BADHANDLE;
            rc = NFS_REQ_ERROR;
            goto done;
        }
        if (nread == 0)
            break;
        for (bpos = 0; bpos < nread;)
        {
            if (to_vfs_dirent(buf, bpos, dentryp, baseloc))
            {
                LOG(MODULE_NAME, D_INFO,
                    "\nvd->vd_ino:%lu\nvd->vd_reclen:%d\nvd->vd_type:%d\nvd->vd_offset:%ld\nvd->vd_name:%s\n",
                    dentryp->vd_ino, dentryp->vd_reclen, dentryp->vd_type, dentryp->vd_offset, dentryp->vd_name);
                node = new entryplus3;
                node->name = (char *)gsh_calloc(strlen(dentryp->vd_name) + 1, sizeof(char));
                memcpy(node->name, dentryp->vd_name, strlen(dentryp->vd_name));
                /*添加结束符*/
                *(node->name + strlen(dentryp->vd_name)) = '\0';
                node->fileid = dentryp->vd_ino;
                node->cookie = (uint64_t)dentryp->vd_offset;
                node->nextentry = nullptr;
                if (strcmp(node->name, ".") == 0 || strcmp(node->name, "..") == 0)
                {
                    node->name_attributes.attributes_follow = FALSE;
                    node->name_handle.handle_follows = FALSE;
                }
                else
                {
                    file_path = string(readdirplus_args->dir.data.data_val) + "/" + string(node->name);
                    set_file_handle(&node->name_handle.post_op_fh3_u.handle, file_path);
                    node->name_handle.handle_follows = TRUE;
                    if (nfs_set_post_op_attr(
                            node->name_handle.post_op_fh3_u.handle.data.data_val,
                            &node->name_attributes) != NFS3_OK)
                    {
                        rc = NFS_REQ_ERROR;
                        LOG(MODULE_NAME, D_ERROR, "nfs_readdirplus 'stat %s' failed",
                            node->name_handle.post_op_fh3_u.handle.data.data_val);
                        goto done;
                    }
                }
                current->nextentry = node;
                current = current->nextentry;
                entry_count++;
                used_mem += sizeof(node);
                if (entry_count >= max_entry_count || (used_mem + BYTES_PER_XDR_UNIT) >= mem_avail)
                {
                    if (node->cookie == LONG_MAX)
                        break;
                    goto page;
                }
            }
            bpos += dentryp->vd_reclen;
        }
    } while (true);

    readdirplus_res_ok->reply.eof = TRUE;

page:
    readdirplus_res_ok->reply.entries = head->nextentry;
    memcpy(readdirplus_res_ok->cookieverf, cookie_verifier, sizeof(cookieverf3));

done:
    close(dir_fh);

out:
    return rc;
}

void nfs3_readdirplus_free(nfs_res_t *res)
{
    /* Nothing to do here */
    // delete entry
    entryplus3 *cur = res->res_readdirplus3.READDIRPLUS3res_u.resok.reply.entries;
    entryplus3 *nxt;
    LOG(MODULE_NAME, D_INFO, "readdirplus free");
    while (cur != nullptr and cur->nextentry != nullptr)
    {
        nxt = cur->nextentry;
        if (cur->name_handle.handle_follows)
        {
            gsh_free(cur->name);
            gsh_free(cur->name_handle.post_op_fh3_u.handle.data.data_val);
        }
        delete (cur);
        cur = nxt;
    }
}

bool xdr_READDIRPLUS3args(XDR *xdrs, READDIRPLUS3args *objp)
{
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

bool xdr_entryplus3(XDR *xdrs, entryplus3 *objp)
{
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
    if (!xdr_pointer(xdrs, (void **)&objp->nextentry, sizeof(entryplus3),
                     (xdrproc_t)xdr_entryplus3))
        return FALSE;
    return TRUE;
}

bool xdr_dirlistplus3(XDR *xdrs, dirlistplus3 *objp)
{
    if (!xdr_pointer(xdrs, (void **)&objp->entries, sizeof(entryplus3),
                     (xdrproc_t)xdr_entryplus3))
        return FALSE;
    if (!xdr_bool(xdrs, &objp->eof))
        return FALSE;
    return TRUE;
}

bool xdr_READDIRPLUS3resok(XDR *xdrs, READDIRPLUS3resok *objp)
{
    if (!xdr_post_op_attr(xdrs, &objp->dir_attributes))
        return FALSE;
    if (!xdr_cookieverf3(xdrs, objp->cookieverf))
        return FALSE;
    if (!xdr_dirlistplus3(xdrs, &objp->reply))
        return FALSE;
    return TRUE;
}

bool xdr_READDIRPLUS3resfail(XDR *xdrs, READDIRPLUS3resfail *objp)
{
    if (!xdr_post_op_attr(xdrs, &objp->dir_attributes))
        return FALSE;
    return TRUE;
}

bool xdr_READDIRPLUS3res(XDR *xdrs, READDIRPLUS3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status)
    {
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