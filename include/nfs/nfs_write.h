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
#ifndef DNFSD_NFS_WRITE_H
#define DNFSD_NFS_WRITE_H

#include "nfs_args.h"

enum stable_how {
    UNSTABLE = 0,
    DATA_SYNC = 1,
    FILE_SYNC = 2
};

struct WRITE3args {
    nfs_fh3 file;
    offset3 offset;
    count3 count;
    stable_how stable;
    struct {
        u_int data_len;
        char *data_val;
    } data;
};

struct WRITE3resok {
    wcc_data file_wcc;
    count3 count;
    stable_how committed;
    writeverf3 verf;
};

struct WRITE3resfail {
    wcc_data file_wcc;
};

struct WRITE3res {
    nfsstat3 status;
    union {
        WRITE3resok resok;
        WRITE3resfail resfail;
    } WRITE3res_u;
};

/*写数据请求*/
struct nfs3_write_data {
    /** 写数据结果 */
    nfs_res_t *res;
    /**写请求*/
    struct svc_req *req;
    /**句柄*/
    struct f_handle *obj;
    /**曹祖结果*/
    int rc;
    /*文件路径*/
    char *file_path;
    /**控制异步标志*/
    uint32_t flags;
    /**需要写入的数据*/
    struct fsal_io_arg write_arg;
};

/*重构函数作为参数*/
typedef void (*fsal_async_cb)(nfs3_write_data *write_data);

/*构造返回结果
 * params data:写操作结果
 * return 操作结果
 * */
int nfs3_complete_write(struct nfs3_write_data *data);

enum xprt_stat nfs3_write_resume(struct svc_req *req);

/*写操作完处理函数
 * params write_data:写请求保存
 * */
void nfs3_write_cb(nfs3_write_data *write_data);

/*写缓存
 * params done_cb:写完数据处理函数
 * params write_arg:待写入数据
 * params write_data:写请求
 * */
void nfs_write_buff(fsal_async_cb done_cb,
                    struct fsal_io_arg *write_arg,
                    nfs3_write_data *write_data);

int nfs3_write(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_write_free(nfs_res_t *res);

bool xdr_stable_how(XDR *xdrs, stable_how *objp);

bool xdr_WRITE3args(XDR *xdrs, WRITE3args *objp);

bool xdr_WRITE3resok(XDR *xdrs, WRITE3resok *objp);

bool xdr_WRITE3resfail(XDR *xdrs, WRITE3resfail *objp);

bool xdr_WRITE3res(XDR *xdrs, WRITE3res *objp);

#endif //DNFSD_NFS_WRITE_H
