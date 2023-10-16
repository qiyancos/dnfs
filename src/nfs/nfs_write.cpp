/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#include "nfs/nfs_write.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_write(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;
    /*文件句柄*/
    int file_handle;

    /*保存操作前的文件信息*/
    struct pre_op_attr pre{};

    /*写入数据大小*/
    ssize_t write_count;

    /*写入数据*/
    iovec write_buf{};

    /*数据指针*/
    WRITE3args *write_args = &arg->arg_write3;
    WRITE3resok *write_res_ok = &res->res_write3.WRITE3res_u.resok;
    WRITE3resfail *write_res_fail = &res->res_write3.WRITE3res_u.resfail;

    if (write_args->file.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_write get file handle len is 0");
        goto out;
    }

    /*打印写入数据*/
    LOG(MODULE_NAME, D_INFO, "write data len is: %d", strlen(write_args->data.data_val));

    get_file_handle(write_args->file);

    /*判断文件存不存在*/
    if (!judge_file_exit(write_args->file.data.data_val, S_IFREG)) {
        rc = NFS_REQ_ERROR;
        res->res_write3.status = NFS3ERR_NOENT;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_write obtained file handle '%s' not exist",
            write_args->file.data.data_val);
        goto out;
    }

    /*获取文件属性信息*/
    res->res_write3.status = get_pre_op_attr(write_args->file.data.data_val,
                                             pre);
    if (res->res_write3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_write failed to obtain '%s' pre_attributes",
            write_args->file.data.data_val);
        goto out;
    }

    /*判定写入大小是否一致*/
    if (write_args->count > write_args->data.data_len) {
        /* should never happen */
        res->res_write3.status = NFS3ERR_INVAL;
        goto out;
    }

    /*todo 判定文件的最大小，数据大小加偏移量 */


    /* We should take care not to exceed FSINFO wtmax field for the size */
    if (write_args->count > 512 * 1024) {
        /* The client asked for too much data, we must restrict him */
        write_args->count = 512 * 1024;
    }

    if (write_args->count == 0) {
        rc=NFS_REQ_OK;
        res->res_write3.status = NFS3_OK;
        goto outok;
    }

    /*获取文件句柄*/
    file_handle = open(write_args->file.data.data_val, O_CREAT | O_WRONLY);
    /*打开失败*/
    if (file_handle == -1) {
        res->res_write3.status = NFS3ERR_NOENT;
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR, "open file '%s' failed",
            write_args->file.data.data_val);
        goto outfail;
    }

    /*构造写入数据*/
    write_buf.iov_len = write_args->data.data_len;
    write_buf.iov_base = calloc(write_args->data.data_len, sizeof(char));
    memcpy(write_buf.iov_base,write_args->data.data_val,write_buf.iov_len);

    /*读取数据*/
    write_count=pwritev(file_handle, &write_buf, 1,(__off_t)write_args->offset);

    /*释放内存*/
    free(write_buf.iov_base);
    /*关闭句柄*/
    close(file_handle);

    /*写入失败*/
    if(write_count<0){
        res->res_write3.status = NFS3ERR_IO;
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR, "open file '%s' failed",
            write_args->file.data.data_val);
        goto outfail;
    }

    /*返回写入数据大小*/
    write_res_ok->count=write_count;

    outok:
    /*获取目录wcc信息*/
    res->res_write3.status = get_wcc_data(write_args->file.data.data_val,
                                          pre,
                                          write_res_ok->file_wcc);
    /*获取弱属性信息失败*/
    if (res->res_write3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface write failed to obtain '%s' resok wcc_data",
            write_args->file.data.data_val);
    }

    goto out;

    outfail:
    /*获取目录wcc信息*/
    res->res_write3.status = get_wcc_data(write_args->file.data.data_val,
                                          pre,
                                          write_res_fail->file_wcc);
    /*获取弱属性信息失败*/
    if (res->res_write3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface write failed to obtain '%s' resfail wcc_data",
            write_args->file.data.data_val);
    }

    out:
    if ((arg->arg_write3.stable == DATA_SYNC) ||
        (arg->arg_write3.stable == FILE_SYNC))
        write_res_ok->committed = FILE_SYNC;
    else
        write_res_ok->committed = UNSTABLE;

    LOG(MODULE_NAME,D_INFO,"Interface write result stat is %d:",res->res_write3.status);

    memcpy(write_res_ok->verf, NFS3_write_verifier, sizeof(writeverf3));
    return rc;
}


void nfs3_write_free(nfs_res_t *res) {

}

bool xdr_stable_how(XDR *xdrs, stable_how *objp) {
    if (!xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_WRITE3args(XDR *xdrs, WRITE3args *objp) {
    struct nfs_request_lookahead *lkhd =
            xdrs->x_public ? (struct nfs_request_lookahead *) xdrs->
                    x_public : &dummy_lookahead;

    if (!xdr_nfs_fh3(xdrs, &objp->file))
        return (false);
    if (!xdr_offset3(xdrs, &objp->offset))
        return (false);
    if (!xdr_count3(xdrs, &objp->count))
        return (false);
    if (!xdr_stable_how(xdrs, &objp->stable))
        return (false);
    if (!xdr_bytes
            (xdrs, (char **) &objp->data.data_val,
             &objp->data.data_len, XDR_BYTES_MAXLEN_IO))
        return (false);
    lkhd->flags |= NFS_LOOKAHEAD_WRITE;
    (lkhd->write)++;
    return (true);
}

bool xdr_WRITE3resok(XDR *xdrs, WRITE3resok *objp) {
    if (!xdr_wcc_data(xdrs, &objp->file_wcc))
        return (false);
    if (!xdr_count3(xdrs, &objp->count))
        return (false);
    if (!xdr_stable_how(xdrs, &objp->committed))
        return (false);
    if (!xdr_writeverf3(xdrs, objp->verf))
        return (false);
    return (true);
}

bool xdr_WRITE3resfail(XDR *xdrs, WRITE3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->file_wcc))
        return (false);
    return (true);
}

bool xdr_WRITE3res(XDR *xdrs, WRITE3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_WRITE3resok(xdrs, &objp->WRITE3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_WRITE3resfail(xdrs, &objp->WRITE3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}