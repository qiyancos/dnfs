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
#ifndef DNFSD_NFS_CREATE_H
#define DNFSD_NFS_CREATE_H

#include "nfs_args.h"

enum createmode3 {
    UNCHECKED = 0,
    GUARDED = 1,
    EXCLUSIVE = 2
};
struct createhow3 {
    createmode3 mode;
    union {
        sattr3 obj_attributes;
        createverf3 verf;
    } createhow3_u;
};

struct CREATE3args {
    diropargs3 where;
    createhow3 how;
};

struct CREATE3resok {
    post_op_fh3 obj;
    post_op_attr obj_attributes;
    wcc_data dir_wcc;
};

struct CREATE3resfail {
    wcc_data dir_wcc;
};

struct CREATE3res {
    nfsstat3 status;
    union {
        CREATE3resok resok;
        CREATE3resfail resfail;
    } CREATE3res_u;
};

int nfs3_create(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_create_free(nfs_res_t *res);

bool xdr_createmode3(XDR *xdrs, createmode3 *objp);

bool xdr_createverf3(XDR *xdrs, createverf3 objp);

bool xdr_createhow3(XDR *xdrs, createhow3 *objp);

bool xdr_CREATE3args(XDR *xdrs, CREATE3args *objp);

bool xdr_CREATE3resok(XDR *xdrs, CREATE3resok *objp);

bool xdr_CREATE3resfail(XDR *xdrs, CREATE3resfail *objp);

bool xdr_CREATE3res(XDR *xdrs, CREATE3res *objp);

#endif //DNFSD_NFS_CREATE_H
