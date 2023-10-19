/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *              Piyuyang pi_yuyang@163.com
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
#ifndef DNFSD_NFS_COMMIT_H
#define DNFSD_NFS_COMMIT_H

#include "nfs_args.h"

struct COMMIT3args
{
    nfs_fh3 file;
    offset3 offset;
    count3 count;
};

struct COMMIT3resok
{
    wcc_data file_wcc;
    writeverf3 verf;
};

struct COMMIT3resfail
{
    wcc_data file_wcc;
};

struct COMMIT3res
{
    nfsstat3 status;
    union
    {
        COMMIT3resok resok;
        COMMIT3resfail resfail;
    } COMMIT3res_u;
};

int nfs3_commit(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_commit_free(nfs_res_t *res);

bool xdr_COMMIT3args(XDR *xdrs, COMMIT3args *objp);

bool xdr_COMMIT3resok(XDR *xdrs, COMMIT3resok *objp);

bool xdr_COMMIT3resfail(XDR *xdrs, COMMIT3resfail *objp);

bool xdr_COMMIT3res(XDR *xdrs, COMMIT3res *objp);

#endif //DNFSD_NFS_COMMIT_H
