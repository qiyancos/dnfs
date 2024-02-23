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
#ifndef DNFSD_NFS_READLINK_H
#define DNFSD_NFS_READLINK_H

#include "nfs_args.h"

struct READLINK3args {
    nfs_fh3 symlink;
};

struct READLINK3resok {
    post_op_attr symlink_attributes;
    nfspath3 data;
};

struct READLINK3resfail {
    post_op_attr symlink_attributes;
};

struct READLINK3res {
    nfsstat3 status;
    union {
        READLINK3resok resok;
        READLINK3resfail resfail;
    } READLINK3res_u;
};

int nfs3_readlink(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_readlink_free(nfs_res_t *res);

bool xdr_READLINK3args(XDR *xdrs, READLINK3args *objp);

bool xdr_READLINK3resok(XDR *xdrs, READLINK3resok *objp);

bool xdr_READLINK3resfail(XDR *xdrs, READLINK3resfail *objp);

bool xdr_READLINK3res(XDR *xdrs, READLINK3res *objp);


#endif //DNFSD_NFS_READLINK_H
