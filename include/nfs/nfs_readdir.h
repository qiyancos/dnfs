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
#ifndef DNFSD_NFS_READDIR_H
#define DNFSD_NFS_READDIR_H

#include "nfs_args.h"

struct READDIR3args
{
    nfs_fh3 dir;
    cookie3 cookie;
    cookieverf3 cookieverf;
    count3 count;
};

struct entry3
{
    fileid3 fileid;
    filename3 name;
    cookie3 cookie;
    struct entry3 *nextentry;
};

struct dirlist3
{
    entry3 *entries;
    bool_t eof;
};

struct READDIR3resok
{
    post_op_attr dir_attributes;
    cookieverf3 cookieverf;
    dirlist3 reply;
};

struct READDIR3resfail
{
    post_op_attr dir_attributes;
};

struct READDIR3res
{
    nfsstat3 status;
    union
    {
        READDIR3resok resok;
        READDIR3resfail resfail;
    } READDIR3res_u;
};

int nfs3_readdir(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_readdir_free(nfs_res_t *res);

bool xdr_READDIR3args(XDR *, READDIR3args *);

bool xdr_READDIR3resok(XDR *, READDIR3resok *);

bool xdr_READDIR3resfail(XDR *, READDIR3resfail *);

bool xdr_READDIR3res(XDR *, READDIR3res *);

#endif // DNFSD_NFS_READDIR_H
