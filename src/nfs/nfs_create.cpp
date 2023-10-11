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
#include "nfs/nfs_create.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "string"

using namespace std;

#define MODULE_NAME "NFS"

int nfs3_create(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*创建文件路径*/
    string file_path;

    /*操作状态*/
    nfsstat3 status;

    /*保存目录操作前属性信息*/
    struct pre_op_attr pre{};

    int file_handle;

    /*数据指针*/
    CREATE3args *create_args = &arg->arg_create3;
    CREATE3resok *create_res_ok = &res->res_create3.CREATE3res_u.resok;
    CREATE3resfail *create_res_fail = &res->res_create3.CREATE3res_u.resfail;

    if (create_args->where.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_create get dir handle len is 0");
        goto out;
    }

    get_file_handle(create_args->where.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_create obtained dir handle is '%s', and the length is '%d'",
        create_args->where.dir.data.data_val,
        create_args->where.dir.data.data_len);

    /*判断创建目录存不存在*/
    if (!judge_file_exit(create_args->where.dir.data.data_val, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        res->res_create3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_create obtained dir handle '%s' not exist",
            create_args->where.dir.data.data_val);
        goto out;
    }

    /*获取之前的属性*/
    res->res_create3.status = get_pre_op_attr(create_args->where.dir.data.data_val,
                                              pre);
    if (res->res_create3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_create failed to obtain '%s' pre_attributes",
            create_args->where.dir.data.data_val);
        goto out;
    }

    /*获取创建路径*/
    file_path =
            string(create_args->where.dir.data.data_val) + "/" + create_args->where.name;

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_create file path is '%s'", file_path.c_str());

    file_handle = open(file_path.c_str(), O_CREAT);

    /*创建文件*/
    if (file_handle == -1) {
        rc = NFS_REQ_ERROR;
        res->res_create3.status = NFS3ERR_NOENT;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_create failed to create file '%s'",
            file_path.c_str());
        goto outfail;
    }

    /*关闭文件*/
    close(file_handle);

    /*创建成功*/
    /*获取文件句柄*/
    set_file_handle(&create_res_ok->obj.post_op_fh3_u.handle,file_path);

    create_res_ok->obj.handle_follows= true;

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_create create file handle is '%s', and the length is '%d'",
        create_res_ok->obj.post_op_fh3_u.handle.data.data_val,
        create_res_ok->obj.post_op_fh3_u.handle.data.data_len);

    /*获取文件属性*/
    res->res_create3.status = nfs_set_post_op_attr(
            create_res_ok->obj.post_op_fh3_u.handle.data.data_val,
            &create_res_ok->obj_attributes);

    if (res->res_create3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_create failed to obtain '%s' resok attributes",
            create_res_ok->obj.post_op_fh3_u.handle.data.data_val);
    }

    /*获取目录wcc信息*/
    res->res_create3.status = get_wcc_data(create_args->where.dir.data.data_val,
                                           pre,
                                           create_res_ok->dir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_create3.status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_create failed to obtain '%s' resok wcc_data",
            create_args->where.dir.data.data_val);
    }

    goto out;

    outfail:
    /*获取失败目录wcc信息*/
    status = get_wcc_data(create_args->where.dir.data.data_val,
                          pre,
                          create_res_fail->dir_wcc);
    /*获取弱属性信息失败*/
    if (status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_create failed to obtain '%s' resfail wcc_data",
            create_args->where.dir.data.data_val);
    }

    out:
    return rc;
}

void nfs3_create_free(nfs_res_t *res) {
    /*释放句柄内存*/
    if ((res->res_create3.status == NFS3_OK)
        && (res->res_create3.CREATE3res_u.resok.obj.handle_follows)) {
        free(res->res_create3.CREATE3res_u.resok.obj.post_op_fh3_u.handle.data.data_val);
    }
}

bool xdr_createmode3(XDR *xdrs, createmode3 *objp) {
    if (!xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_createverf3(XDR *xdrs, createverf3 objp) {
    if (!xdr_opaque(xdrs, objp, 8))
        return (false);
    return (true);
}

bool xdr_createhow3(XDR *xdrs, createhow3 *objp) {
    if (!xdr_createmode3(xdrs, &objp->mode))
        return (false);
    switch (objp->mode) {
        case UNCHECKED:
        case GUARDED:
            if (!xdr_sattr3(xdrs, &objp->createhow3_u.obj_attributes))
                return (false);
            break;
        case EXCLUSIVE:
            if (!xdr_createverf3(xdrs, objp->createhow3_u.verf))
                return (false);
            break;
        default:
            return (false);
    }
    return (true);
}

bool xdr_CREATE3args(XDR *xdrs, CREATE3args *objp) {
    struct nfs_request_lookahead *lkhd =
            xdrs->x_public ? (struct nfs_request_lookahead *) xdrs->
                    x_public : &dummy_lookahead;

    if (!xdr_diropargs3(xdrs, &objp->where))
        return (false);
    if (!xdr_createhow3(xdrs, &objp->how))
        return (false);
    lkhd->flags |= NFS_LOOKAHEAD_CREATE;
    return (true);
}

bool xdr_CREATE3resok(XDR *xdrs, CREATE3resok *objp) {
    if (!xdr_post_op_fh3(xdrs, &objp->obj))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_CREATE3resfail(XDR *xdrs, CREATE3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_CREATE3res(XDR *xdrs, CREATE3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_CREATE3resok(xdrs, &objp->CREATE3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_CREATE3resfail(xdrs, &objp->CREATE3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}