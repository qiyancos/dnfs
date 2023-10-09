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
#ifndef DNFSD_NFS_WRITE_H
#define DNFSD_NFS_WRITE_H
#include "nfs_args.h"

enum stable_how {
    UNSTABLE = 0,
    DATA_SYNC = 1,
    FILE_SYNC = 2
};

struct WRITE3args {
    nfs_fh3 file;
    offset3 offset;
    count3 count;
    stable_how stable;
    struct {
        u_int data_len;
        char *data_val;
    } data;
};

struct WRITE3resok {
    wcc_data file_wcc;
    count3 count;
    stable_how committed;
    writeverf3 verf;
};

struct WRITE3resfail {
    wcc_data file_wcc;
};

struct WRITE3res {
    nfsstat3 status;
    union {
        WRITE3resok resok;
        WRITE3resfail resfail;
    } WRITE3res_u;
};

/*声明数据参数*/
union nfs_arg_t;
union nfs_res_t;

int nfs3_write(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_write_free(nfs_res_t *res);

extern bool xdr_stable_how(XDR *xdrs, stable_how *objp);
extern bool xdr_WRITE3args(XDR *xdrs, WRITE3args *objp);
extern bool xdr_WRITE3resok(XDR *xdrs, WRITE3resok *objp);
extern bool xdr_WRITE3resfail(XDR *xdrs, WRITE3resfail *objp);
extern bool xdr_WRITE3res(XDR *xdrs, WRITE3res *objp);

#endif //DNFSD_NFS_WRITE_H
