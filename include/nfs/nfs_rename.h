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
#ifndef DNFSD_NFS_RENAME_H
#define DNFSD_NFS_RENAME_H

#include "nfs_xdr.h"
#include "nfs_args.h"

struct RENAME3args
{
    diropargs3 from;
    diropargs3 to;
};

struct RENAME3resok
{
    wcc_data fromdir_wcc;
    wcc_data todir_wcc;
};

struct RENAME3resfail
{
    wcc_data fromdir_wcc;
    wcc_data todir_wcc;
};

struct RENAME3res
{
    nfsstat3 status;
    union
    {
        RENAME3resok resok;
        RENAME3resfail resfail;
    } RENAME3res_u;
};

int nfs3_rename(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_rename_free(nfs_res_t *res);

extern bool xdr_RENAME3args(XDR *, RENAME3args *);

extern bool xdr_RENAME3resok(XDR *, RENAME3resok *);

extern bool xdr_RENAME3resfail(XDR *, RENAME3resfail *);

extern bool xdr_RENAME3res(XDR *, RENAME3res *);

#endif // DNFSD_NFS_RENAME_H
