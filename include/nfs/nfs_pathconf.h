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
#ifndef DNFSD_NFS_PATHCONF_H
#define DNFSD_NFS_PATHCONF_H

#include "nfs_args.h"

struct PATHCONF3args {
    nfs_fh3 object;
};

struct PATHCONF3resok {
    post_op_attr obj_attributes;
    nfs3_uint32 linkmax;
    nfs3_uint32 name_max;
    bool_t no_trunc;
    bool_t chown_restricted;
    bool_t case_insensitive;
    bool_t case_preserving;
};

struct PATHCONF3resfail {
    post_op_attr obj_attributes;
};

struct PATHCONF3res {
    nfsstat3 status;
    union {
        PATHCONF3resok resok;
        PATHCONF3resfail resfail;
    } PATHCONF3res_u;
};

int nfs3_pathconf(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_pathconf_free(nfs_res_t *res);

extern bool xdr_PATHCONF3args(XDR *xdrs, PATHCONF3args *objp);

extern bool xdr_PATHCONF3resok(XDR *xdrs, PATHCONF3resok *objp);

extern bool xdr_PATHCONF3resfail(XDR *xdrs, PATHCONF3resfail *objp);

extern bool xdr_PATHCONF3res(XDR *xdrs, PATHCONF3res *objp);

#endif //DNFSD_NFS_PATHCONF_H
