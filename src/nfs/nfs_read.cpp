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
#include <sys/uio.h>

#include "nfs/nfs_read.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

static void nfs_read_ok(nfs_res_t *res, char *data, uint32_t read_size,
                        char *file_path, int eof)
{
    if ((read_size == 0) && (data != NULL))
    {
        free(data);
        data = NULL;
    }

    /* Build Post Op Attributes */
    nfs_set_post_op_attr(file_path, &res->res_read3.READ3res_u.resok.file_attributes);

    res->res_read3.READ3res_u.resok.eof = eof;
    res->res_read3.READ3res_u.resok.count = read_size;
    res->res_read3.READ3res_u.resok.data.data_val = data;
    res->res_read3.READ3res_u.resok.data.data_len = read_size;
}

int nfs3_read(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    /*数据指针*/
    READ3args *read_args = &arg->arg_read3;
    uint64_t offset = read_args->offset;
    size_t size = read_args->count;
    READ3resok *read_res_ok = &res->res_read3.READ3res_u.resok;
    READ3resfail *read_res_fail = &res->res_read3.READ3res_u.resfail;

    int rc = NFS_REQ_OK;
    int fd;
    ssize_t nb_read;
    int iov_count = 1;           /**< Number of vectors in iov */
    struct iovec iov[iov_count]; /**< Vector of buffers to fill */
    size_t io_amount;
    bool end_of_file;
    // todo 从配置文件读取，暂时给固定值
    uint64_t MaxRead = 512 * 1024;
    uint64_t MaxOffsetRead = UINT64_MAX;

    /* to avoid setting it on each error case */
    read_res_fail->file_attributes.attributes_follow = FALSE;

    /* initialize for read of size 0 */
    read_res_ok->eof = FALSE;
    read_res_ok->count = 0;
    read_res_ok->data.data_val = NULL;
    read_res_ok->data.data_len = 0;
    res->res_read3.status = NFS3_OK;

    if (read_args->file.data.data_len == 0)
    {
        res->res_read3.status = NFS3ERR_BADHANDLE;
        LOG(MODULE_NAME, D_ERROR, "arg_read get file handle len is 0");
        goto out;
    }

    get_file_handle(read_args->file);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_read obtained file handle is '%s', and the length is '%d'",
        read_args->file.data.data_val,
        read_args->file.data.data_len);

    res->res_read3.status = nfs_set_post_op_attr(
        read_args->file.data.data_val,
        &read_res_ok->file_attributes);
    if (res->res_read3.status != NFS3_OK)
    {
        LOG(MODULE_NAME, D_ERROR, "stat '%s' failed",
            read_args->file.data.data_val);
        res->res_read3.status = NFS3ERR_BADHANDLE;
        goto out;
    }
    if (read_res_ok->file_attributes.post_op_attr_u.attributes.type != NF3REG)
    {
        if (read_res_ok->file_attributes.post_op_attr_u.attributes.type == NF3DIR)
            res->res_read3.status = NFS3ERR_ISDIR;
        else
            res->res_read3.status = NFS3ERR_INVAL;
        goto out;
    }

    LOG(MODULE_NAME, D_INFO, "read file start %u, len %zu", offset, size);

    /* do not exceed maximum READ offset if set */
    if (MaxOffsetRead < UINT64_MAX)
    {
        LOG(MODULE_NAME, D_INFO,
            "Read offset=%u count=%zd MaxOffSet=%",
            offset, size, MaxOffsetRead);
        if ((offset + size) > MaxOffsetRead)
        {
            LOG(MODULE_NAME, D_INFO,
                "A client tried to violate max file size %u",
                MaxOffsetRead);
            res->res_read3.status = NFS3ERR_FBIG;
            goto outfail;
        }
    }

    /* We should not exceed the FSINFO rtmax field for the size */
    if (size > MaxRead)
    {
        /* The client asked for too much, normally this should
           not happen because the client is calling nfs_Fsinfo
           at mount time and so is aware of the server maximum
           write size */
        size = MaxRead;
    }

    if (size == 0)
    {
        nfs_read_ok(res, NULL, 0, read_args->file.data.data_val, 0);
        goto out;
    }

    iov[0].iov_len = size;
    iov[0].iov_base = malloc(RNDUP(size));
    fd = open(read_args->file.data.data_val, O_RDONLY);

    if (fd == -1)
    {
        rc = NFS_REQ_ERROR;
        res->res_read3.status = NFS3ERR_NOENT;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_read failed to open file '%s'",
            read_args->file.data.data_val);
        free(iov[0].iov_base);
        goto outfail;
    }

    nb_read = preadv(fd, iov, iov_count, offset);
    close(fd);

    if (offset == -1 || nb_read == -1)
    {
        rc = NFS_REQ_ERROR;
        res->res_read3.status = NFS3ERR_IO;
        LOG(MODULE_NAME, D_ERROR, "preadv failed returning %s", errno);
        
        free(iov[0].iov_base);
        goto outfail;
    }

    io_amount = nb_read;
    end_of_file = (offset + io_amount) >= read_res_ok->file_attributes.post_op_attr_u.attributes.size;
    nfs_read_ok(res, (char *)iov[0].iov_base, io_amount,
                read_args->file.data.data_val, end_of_file);

out:
    return rc;

outfail:
    if (nfs_set_post_op_attr(read_args->file.data.data_val, &read_res_fail->file_attributes) != NFS3_OK)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "Interface nfs_read failed to obtain '%s' file_attributes",
            read_args->file.data.data_val);
    }
    goto out;
}

void nfs3_read_free(nfs_res_t *res)
{
    if (res->res_read3.status == NFS3_OK)
        free(res->res_read3.READ3res_u.resok.data.data_val);
}

bool xdr_READ3args(XDR *xdrs, READ3args *objp)
{
    if (!xdr_nfs_fh3(xdrs, &objp->file))
        return FALSE;
    if (!xdr_offset3(xdrs, &objp->offset))
        return FALSE;
    if (!xdr_count3(xdrs, &objp->count))
        return FALSE;
    return TRUE;
}

bool xdr_READ3resok(XDR *xdrs, READ3resok *objp)
{
    if (!xdr_post_op_attr(xdrs, &objp->file_attributes))
        return FALSE;
    if (!xdr_count3(xdrs, &objp->count))
        return FALSE;
    if (!xdr_bool(xdrs, &objp->eof))
        return FALSE;
    if (!xdr_bytes(xdrs, (char **)&objp->data.data_val, (u_int *)&objp->data.data_len, XDR_BYTES_MAXLEN_IO))
        return FALSE;
    return TRUE;
}

bool xdr_READ3resfail(XDR *xdrs, READ3resfail *objp)
{
    if (!xdr_post_op_attr(xdrs, &objp->file_attributes))
        return FALSE;
    return TRUE;
}

bool xdr_READ3res(XDR *xdrs, READ3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status)
    {
    case NFS3_OK:
        if (!xdr_READ3resok(xdrs, &objp->READ3res_u.resok))
            return FALSE;
        break;
    default:
        if (!xdr_READ3resfail(xdrs, &objp->READ3res_u.resfail))
            return FALSE;
        break;
    }
    return TRUE;
}

#define MODULE_NAME "NFS"