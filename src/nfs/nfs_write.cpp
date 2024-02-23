/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */
#include <sys/uio.h>
#include "nfs/nfs_write.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "file/fsal_handle.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

/*构造返回结果
 * params data:写操作结果
 * return 操作结果
 * */
int nfs3_complete_write(struct nfs3_write_data *data) {
    struct fsal_io_arg *write_arg = &data->write_arg;
    WRITE3resok *write_res_ok = &data->res->res_write3.WRITE3res_u.resok;
    WRITE3resfail *write_res_fail = &data->res->res_write3.WRITE3res_u.resfail;
    pre_op_attr pre{};

    if (data->rc == NFS_REQ_OK) {
        /* 获取弱属性信息 */
        data->res->res_write3.status = get_wcc_data(data->file_path, pre,
                                                    write_res_ok->file_wcc);
        /*获取弱属性信息失败*/
        if (data->res->res_write3.status != NFS3_OK) {
            LOG(MODULE_NAME, D_ERROR,
                "Interface write failed to obtain '%s' resok wcc_data",
                data->file_path);
        }

        /* 设置写入数据大小 */
        write_res_ok->count = write_arg->io_amount;

        /* 判断数据提交方式*/
        if (write_arg->fsal_stable)
            write_res_ok->committed = FILE_SYNC;
        else
            write_res_ok->committed = UNSTABLE;

        /* 设置系统验证*/
        memcpy(write_res_ok->verf, NFS3_write_verifier, sizeof(writeverf3));
    } else if (data->rc == NFS_REQ_ERROR) {
        /* 获取失败的弱属性验证 */
        data->res->res_write3.status = get_wcc_data(data->file_path, pre,
                                                    write_res_fail->file_wcc);

        /*获取弱属性信息失败*/
        if (data->res->res_write3.status != NFS3_OK) {
            LOG(MODULE_NAME, D_ERROR,
                "Interface write failed to obtain '%s' resfail wcc_data",
                data->file_path);
        }

        /* 转变结果 */
        data->rc = NFS_REQ_OK;
    }

    return data->rc;
}

/*写缓存
 * params done_cb:写完数据处理函数
 * params write_arg:待写入数据
 * params write_data:写请求
 * */
void nfs_write_buff(struct fsal_io_arg *write_arg,
                    nfs3_write_data *write_data) {

    /*获取句柄*/
    f_handle *file_handle = write_data->obj;

    fsal_status_t status, status2;

    /*推送缓存结果*/
    int retval;

    status = fsal_start_io(file_handle, write_arg->state, FSAL_O_WRITE, false,
                           file_handle->share);

    if (FSAL_IS_ERROR(status)) {
        LOG(MODULE_NAME, D_ERROR,
            "fsal_start_io failed returning %s",
            fsal_err_txt(status));
        goto out;
    }

    /*读取数据*/
    write_arg->io_amount = pwritev(file_handle->handle, write_arg->iov,
                                   write_arg->iov_count,
                                   (off64_t) write_arg->offset);

    /*写入失败*/
    if (write_arg->io_amount == -1) {
        write_data->res->res_write3.status = NFS3ERR_IO;
        write_data->rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR, "Write file '%s' failed",
            write_data->file_path);
        goto out;
    }

    /*不是异步的，直接刷缓存*/
    if (write_arg->fsal_stable) {
        retval = fsync(file_handle->handle);
        if (retval == -1) {
            write_data->rc = NFS_REQ_ERROR;
            write_data->res->res_write3.status = NFS3ERR_IO;
            LOG(MODULE_NAME, D_ERROR,
                "Interface write failed to refresh file '%s' store",
                write_data->file_path);
            goto out;
        }
    }

    status2 = fsal_complete_io(file_handle);

    LOG(MODULE_NAME, D_INFO,
        "fsal_complete_io returned %s",
        fsal_err_txt(status2));

    if (write_arg->state == nullptr) {
        update_share_counters_locked(file_handle,
                                     FSAL_O_WRITE, FSAL_O_CLOSED);
    }
    out:
    LOG(MODULE_NAME, D_INFO, "Write buff end");
}

int nfs3_write(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;
    /*文件句柄*/
    f_handle *file_handle = nullptr;

    /*保存操作前的文件信息*/
    struct pre_op_attr pre{};

    /*保存写请求*/
    struct nfs3_write_data *write_data;
    /*重构写数据*/
    struct fsal_io_arg *write_arg;
    /*获取写请求*/
    nfs_request_t *reqdata = get_parent_struct_addr(req, nfs_request_t, svc);

    /*数据指针*/
    WRITE3args *write_args = &arg->arg_write3;
    WRITE3resok *write_res_ok = &res->res_write3.WRITE3res_u.resok;
    WRITE3resfail *write_res_fail = &res->res_write3.WRITE3res_u.resfail;

    if (write_args->file.data.data_len == 0) {
        LOG(MODULE_NAME, D_ERROR,
            "arg_write get file handle len is 0");
        goto return_res;
    }

    /*打印写入数据*/
    LOG(MODULE_NAME, D_INFO, "write data len is: %d ,offset is %d,count is %d",
        write_args->data.data_len, write_args->offset, write_args->count);

    get_file_handle(write_args->file);

    /*打印句柄*/
    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_write obtained file handle is '%s', and the length is '%d'",
        write_args->file.data.data_val,
        write_args->file.data.data_len);

    /*判断文件存不存在*/
    if (!judge_file_exit(write_args->file.data.data_val, S_IFREG)) {
        res->res_write3.status = NFS3ERR_INVAL;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_write obtained file handle '%s' not exist",
            write_args->file.data.data_val);
        goto return_res;
    }

    /*判定写入大小是否一致*/
    if (write_args->count > write_args->data.data_len) {
        /* should never happen */
        res->res_write3.status = NFS3ERR_INVAL;
        goto return_res;
    }

    /*todo 判定文件的最大小，数据大小加偏移量 */


    /* We should take care not to exceed FSINFO wtmax field for the size */
    if (write_args->count > WRITE_READ_MAX) {
        /* The client asked for too much data, we must restrict him */
        write_args->count = WRITE_READ_MAX;
    }

    if (write_args->count == 0) {
        res->res_write3.status = NFS3_OK;
        /*获取目录wcc信息*/
        res->res_write3.status = get_wcc_data(write_args->file.data.data_val,
                                              pre,
                                              write_res_ok->file_wcc);

        /*获取弱属性信息失败*/
        if (res->res_write3.status != NFS3_OK) {
            LOG(MODULE_NAME, D_ERROR,
                "Interface write failed to obtain '%s' resok wcc_data",
                write_args->file.data.data_val);
        }
        goto return_res;
    }

    /*获取文件句柄*/
    file_handle = fsal_handle.get_handle(write_args->file.data.data_val);
    /*打开失败*/
    if (file_handle->handle == -1) {
        res->res_write3.status = NFS3ERR_NOENT;
        /*获取目录wcc信息*/
        res->res_write3.status = get_wcc_data(write_args->file.data.data_val,
                                              pre,
                                              write_res_fail->file_wcc);
        /*获取弱属性信息失败*/
        if (res->res_write3.status != NFS3_OK) {
            LOG(MODULE_NAME, D_ERROR,
                "Interface write failed to obtain '%s' resfail wcc_data",
                write_args->file.data.data_val);
        }
        goto return_res;
    }

    /* Set up args, allocate from heap, iov_count will be 1 */
    write_data = (nfs3_write_data *) gsh_calloc(1, sizeof(*write_data) +
                                                   sizeof(struct iovec));
    write_arg = &write_data->write_arg;
    /*读写标志*/
    write_arg->state = nullptr;
    /*写偏移*/
    write_arg->offset = write_args->offset;
    /*是否异步*/
    write_arg->fsal_stable = arg->arg_write3.stable != UNSTABLE;
    /*缓存数目*/
    write_arg->iov_count = 1;
    /*缓存数据*/
    write_arg->iov[0].iov_len = arg->arg_write3.count;
    write_arg->iov[0].iov_base = arg->arg_write3.data.data_val;
    /*写入的数据*/
    write_arg->io_amount = 0;

    /*保存请求参数*/
    write_data->res = res;
    write_data->req = req;

    /*句柄*/
    write_data->obj = file_handle;
    /*文件路径*/
    write_data->file_path = arg->arg_write3.file.data.data_val;

    reqdata->proc_data = write_data;

    nfs_write_buff(write_arg, write_data);

    /* 构造返回结果 */
    rc = nfs3_complete_write(write_data);

    /* Since we're actually done, we can free write_data. */
    gsh_free(write_data);
    reqdata->proc_data = nullptr;

    return_res:
    if (file_handle != nullptr) {
        /*关闭句柄*/
        FsalHandle::close_handle(file_handle);
    }

    LOG(MODULE_NAME, D_INFO, "Interface write result stat is %d:",
        res->res_write3.status);
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