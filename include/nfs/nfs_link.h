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
#ifndef DNFSD_NFS_LINK_H
#define DNFSD_NFS_LINK_H

#include "nfs_args.h"

struct LINK3args {
    nfs_fh3 file;
    diropargs3 link;
};

struct LINK3resok {
    post_op_attr file_attributes;
    wcc_data linkdir_wcc;
};

struct LINK3resfail {
    post_op_attr file_attributes;
    wcc_data linkdir_wcc;
};

struct LINK3res {
    nfsstat3 status;
    union {
        LINK3resok resok;
        LINK3resfail resfail;
    } LINK3res_u;
};

int nfs3_link(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_link_free(nfs_res_t *resp);

bool xdr_LINK3args(XDR *xdrs, LINK3args *objp);

bool xdr_LINK3resok(XDR *xdrs, LINK3resok *objp);

bool xdr_LINK3resfail(XDR *xdrs, LINK3resfail *objp);

bool xdr_LINK3res(XDR *xdrs, LINK3res *objp);

#endif //DNFSD_NFS_LINK_H
