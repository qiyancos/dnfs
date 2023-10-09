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
#ifndef DNFSD_NFS_FSINFO_H
#define DNFSD_NFS_FSINFO_H

#include "nfs_args.h"

struct FSINFO3resok {
    post_op_attr obj_attributes;
    nfs3_uint32 rtmax;
    nfs3_uint32 rtpref;
    nfs3_uint32 rtmult;
    nfs3_uint32 wtmax;
    nfs3_uint32 wtpref;
    nfs3_uint32 wtmult;
    nfs3_uint32 dtpref;
    size3 maxfilesize;
    nfstime3 time_delta;
    nfs3_uint32 properties;
};

struct FSINFO3resfail {
    post_op_attr obj_attributes;
};

struct FSINFO3res {
    nfsstat3 status;
    union {
        FSINFO3resok resok;
        FSINFO3resfail resfail;
    } FSINFO3res_u;
};

struct FSINFO3args {
    nfs_fh3 fsroot;
};

/*声明数据参数*/
union nfs_arg_t;
union nfs_res_t;
/*
 * This function Implements NFSPROC3_FSINFO.
 * */
int nfs3_fsinfo(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

/*
 * Free the result structure allocated for nfs3_fsinfo.
 */
void nfs3_fsinfo_free(nfs_res_t *res);

extern bool xdr_FSINFO3args(XDR *, FSINFO3args *);
extern bool xdr_FSINFO3resok(XDR *, FSINFO3resok *);
extern bool xdr_FSINFO3resfail(XDR *, FSINFO3resfail *);
extern bool xdr_FSINFO3res(XDR *, FSINFO3res *);

#endif //DNFSD_NFS_FSINFO_H
