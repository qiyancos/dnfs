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
#ifndef DNFSD_NFS_SETATTR_H
#define DNFSD_NFS_SETATTR_H

#include "nfs_args.h"

struct sattrguard3 {
    bool_t check;
    union {
        nfstime3 obj_ctime;
    } sattrguard3_u;
};

struct SETATTR3args {
    nfs_fh3 object;
    sattr3 new_attributes;
    sattrguard3 guard;
};

struct SETATTR3resok {
    wcc_data obj_wcc;
};

struct SETATTR3resfail {
    wcc_data obj_wcc;
};

struct SETATTR3res {
    nfsstat3 status;
    union {
        SETATTR3resok resok;
        SETATTR3resfail resfail;
    } SETATTR3res_u;
};

int nfs3_setattr(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_setattr_free(nfs_res_t *res);

extern bool xdr_sattrguard3(XDR *xdrs, sattrguard3 *objp);

extern bool xdr_SETATTR3args(XDR *xdrs, SETATTR3args *objp);

extern bool xdr_SETATTR3resok(XDR *xdrs, SETATTR3resok *objp);

extern bool xdr_SETATTR3resfail(XDR *xdrs, SETATTR3resfail *objp);

extern bool xdr_SETATTR3res(XDR *xdrs, SETATTR3res *objp);

#endif //DNFSD_NFS_SETATTR_H
