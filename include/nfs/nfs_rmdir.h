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
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */
#ifndef DNFSD_NFS_RMDIR_H
#define DNFSD_NFS_RMDIR_H

#include "nfs_args.h"

struct RMDIR3args
{
    diropargs3 object;
};

struct RMDIR3resok
{
    wcc_data dir_wcc;
};

struct RMDIR3resfail
{
    wcc_data dir_wcc;
};

struct RMDIR3res
{
    nfsstat3 status;
    union
    {
        RMDIR3resok resok;
        RMDIR3resfail resfail;
    } RMDIR3res_u;
};

int nfs3_rmdir(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_rmdir_free(nfs_res_t *res);

bool xdr_RMDIR3args(XDR *, RMDIR3args *);

bool xdr_RMDIR3resok(XDR *, RMDIR3resok *);

bool xdr_RMDIR3resfail(XDR *, RMDIR3resfail *);

bool xdr_RMDIR3res(XDR *, RMDIR3res *);

#endif // DNFSD_NFS_RMDIR_H
