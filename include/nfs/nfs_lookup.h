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
#ifndef DNFSD_NFS_LOOKUP_H
#define DNFSD_NFS_LOOKUP_H

#include "nfs_args.h"

struct LOOKUP3args {
    diropargs3 what;
};
struct LOOKUP3resok {
    nfs_fh3 object;
    post_op_attr obj_attributes;
    post_op_attr dir_attributes;
};
struct LOOKUP3resfail {
    post_op_attr dir_attributes;
};

struct LOOKUP3res {
    nfsstat3 status;
    union {
        LOOKUP3resok resok;
        LOOKUP3resfail resfail;
    } LOOKUP3res_u;
};

int nfs3_lookup(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_lookup_free(nfs_res_t *res);

bool xdr_LOOKUP3args(XDR *xdrs, LOOKUP3args *objp);

bool xdr_LOOKUP3resok(XDR *xdrs, LOOKUP3resok *objp);

bool xdr_LOOKUP3resfail(XDR *xdrs, LOOKUP3resfail *objp);

bool xdr_LOOKUP3res(XDR *xdrs, LOOKUP3res *objp);

#endif //DNFSD_NFS_LOOKUP_H
