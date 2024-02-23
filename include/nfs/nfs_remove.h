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
#ifndef DNFSD_NFS_REMOVE_H
#define DNFSD_NFS_REMOVE_H

#include "nfs_args.h"

struct REMOVE3args
{
    diropargs3 object;
};

struct REMOVE3resok
{
    wcc_data dir_wcc;
};

struct REMOVE3resfail
{
    wcc_data dir_wcc;
};

struct REMOVE3res
{
    nfsstat3 status;
    union
    {
        REMOVE3resok resok;
        REMOVE3resfail resfail;
    } REMOVE3res_u;
};

int nfs3_remove(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_remove_free(nfs_res_t *res);

bool xdr_REMOVE3args(XDR *, REMOVE3args *);

bool xdr_REMOVE3resok(XDR *, REMOVE3resok *);

bool xdr_REMOVE3resfail(XDR *, REMOVE3resfail *);

bool xdr_REMOVE3res(XDR *, REMOVE3res *);

#endif // DNFSD_NFS_REMOVE_H
