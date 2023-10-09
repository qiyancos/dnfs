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
#ifndef DNFSD_NFS_ACCESS_H
#define DNFSD_NFS_ACCESS_H
#include "nfs_args.h"

struct ACCESS3args {
    nfs_fh3 object;
    nfs3_uint32 access;
};

struct ACCESS3resok {
    post_op_attr obj_attributes;
    nfs3_uint32 access;
};

struct ACCESS3resfail {
    post_op_attr obj_attributes;
};

struct ACCESS3res {
    nfsstat3 status;
    union {
        ACCESS3resok resok;
        ACCESS3resfail resfail;
    } ACCESS3res_u;
};

/*声明数据参数*/
union nfs_arg_t;
union nfs_res_t;

int nfs3_access(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);
void nfs3_access_free(nfs_res_t *res);

extern bool xdr_ACCESS3args(XDR *xdrs, ACCESS3args *objp);
extern bool xdr_ACCESS3resok(XDR *xdrs, ACCESS3resok *objp);
extern bool xdr_ACCESS3resfail(XDR *xdrs, ACCESS3resfail *objp);
extern bool xdr_ACCESS3res(XDR *xdrs, ACCESS3res *objp);


#endif //DNFSD_NFS_ACCESS_H
