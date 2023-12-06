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
#ifndef DNFSD_NFS_FSSTAT_H
#define DNFSD_NFS_FSSTAT_H

#include "nfs_args.h"

struct FSSTAT3args {
    nfs_fh3 fsroot;
};

struct FSSTAT3resok {
    post_op_attr obj_attributes;
    size3 tbytes;
    size3 fbytes;
    size3 abytes;
    size3 tfiles;
    size3 ffiles;
    size3 afiles;
    nfs3_uint32 invarsec;
};
struct FSSTAT3resfail {
    post_op_attr obj_attributes;
};

struct FSSTAT3res {
    nfsstat3 status;
    union {
        FSSTAT3resok resok;
        FSSTAT3resfail resfail;
    } FSSTAT3res_u;
};

int nfs3_fsstat(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_fsstat_free(nfs_res_t *res);

bool xdr_FSSTAT3args(XDR *xdrs, FSSTAT3args *objp);

bool xdr_FSSTAT3resok(XDR *xdrs, FSSTAT3resok *objp);

bool xdr_FSSTAT3resfail(XDR *xdrs, FSSTAT3resfail *objp);

bool xdr_FSSTAT3res(XDR *xdrs, FSSTAT3res *objp);

#endif //DNFSD_NFS_FSSTAT_H
