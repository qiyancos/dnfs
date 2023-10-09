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
#ifndef DNFSD_NFS_READDIRPLUS_H
#define DNFSD_NFS_READDIRPLUS_H

#include "nfs_xdr.h"
#include "nfs_args.h"

struct READDIRPLUS3args
{
    nfs_fh3 dir;
    cookie3 cookie;
    cookieverf3 cookieverf;
    count3 dircount;
    count3 maxcount;
};
typedef struct READDIRPLUS3args READDIRPLUS3args;

struct entryplus3
{
    fileid3 fileid;
    filename3 name;
    cookie3 cookie;
    post_op_attr name_attributes;
    post_op_fh3 name_handle;
    struct entryplus3 *nextentry;
};
typedef struct entryplus3 entryplus3;

struct dirlistplus3
{
    entryplus3 *entries;
    bool_t eof;
};
typedef struct dirlistplus3 dirlistplus3;

struct READDIRPLUS3resok
{
    post_op_attr dir_attributes;
    cookieverf3 cookieverf;
    dirlistplus3 reply;
};
typedef struct READDIRPLUS3resok READDIRPLUS3resok;

struct READDIRPLUS3resfail
{
    post_op_attr dir_attributes;
};
typedef struct READDIRPLUS3resfail READDIRPLUS3resfail;

struct READDIRPLUS3res
{
    nfsstat3 status;
    union
    {
        READDIRPLUS3resok resok;
        READDIRPLUS3resfail resfail;
    } READDIRPLUS3res_u;
};
typedef struct READDIRPLUS3res READDIRPLUS3res;

/*声明数据参数*/
union nfs_arg_t;
union nfs_res_t;
/*
 * This function Implements NFSPROC3_READDIRPLUS.
 * */
int nfs3_readdirplus(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

/*
 * Free the result structure allocated for nfs3_readdirplus.
 */
void nfs3_readdirplus_free(nfs_res_t *res);

extern bool xdr_READDIRPLUS3args(XDR *, READDIRPLUS3args *);
extern bool xdr_READDIRPLUS3resok(XDR *, READDIRPLUS3resok *);
extern bool xdr_READDIRPLUS3resfail(XDR *, READDIRPLUS3resfail *);
extern bool xdr_READDIRPLUS3res(XDR *, READDIRPLUS3res *);

#endif // DNFSD_NFS_READDIRPLUS_H
