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
#ifndef DNFSD_NFS_SYMLINK_H
#define DNFSD_NFS_SYMLINK_H

#include "nfs/nfs_args.h"

struct symlinkdata3 {
    sattr3 symlink_attributes;
    nfspath3 symlink_data;
};

struct SYMLINK3args {
    diropargs3 where;
    symlinkdata3 symlink;
};

struct SYMLINK3resok {
    post_op_fh3 obj;
    post_op_attr obj_attributes;
    wcc_data dir_wcc;
};

struct SYMLINK3resfail {
    wcc_data dir_wcc;
};

struct SYMLINK3res {
    nfsstat3 status;
    union {
        SYMLINK3resok resok;
        SYMLINK3resfail resfail;
    } SYMLINK3res_u;
};

int nfs3_symlink(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_symlink_free(nfs_res_t *res);

extern bool xdr_symlinkdata3(XDR *xdrs, symlinkdata3 *objp);
extern bool xdr_SYMLINK3args(XDR *xdrs, SYMLINK3args *objp);
extern bool xdr_SYMLINK3resok(XDR *xdrs, SYMLINK3resok *objp);
extern bool xdr_SYMLINK3resfail(XDR *xdrs, SYMLINK3resfail *objp);
extern bool xdr_SYMLINK3res(XDR *xdrs, SYMLINK3res *objp);

#endif //DNFSD_NFS_SYMLINK_H
