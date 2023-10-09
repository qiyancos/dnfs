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
#ifndef DNFSD_NFS_READ_H
#define DNFSD_NFS_READ_H

#include "nfs_args.h"

struct READ3args {
    nfs_fh3 file;
    offset3 offset;
    count3 count;
};

struct READ3resok {
    post_op_attr file_attributes;
    count3 count;
    bool_t eof;
    struct {
        u_int data_len;
        char *data_val;
    } data;
};

struct READ3resfail {
    post_op_attr file_attributes;
};

struct READ3res {
    nfsstat3 status;
    union {
        READ3resok resok;
        READ3resfail resfail;
    } READ3res_u;
};

int nfs3_read(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_read_free(nfs_res_t *res);

extern bool xdr_READ3args(XDR *xdrs, READ3args *objp);

extern void xdr_READ3res_uio_release(struct xdr_uio *uio, u_int flags);

extern xdr_uio *xdr_READ3res_uio_setup(struct READ3resok *objp);

extern inline bool xdr_READ3resok_encode(XDR *xdrs, READ3resok *objp);

extern bool xdr_READ3resok(XDR *xdrs, READ3resok *objp);

extern bool xdr_READ3resfail(XDR *xdrs, READ3resfail *objp);

extern bool xdr_READ3res(XDR *xdrs, READ3res *objp);

#endif //DNFSD_NFS_READ_H
