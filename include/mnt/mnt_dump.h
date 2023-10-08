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
#ifndef DNFSD_MNT_DUMP_H
#define DNFSD_MNT_DUMP_H

#include "mnt_args.h"
/*声明数据参数*/
union nfs_arg_t;
union nfs_res_t;
typedef struct exportnode exportnode;

typedef struct mountbody *mountlist;

struct mountbody {
    mnt3_name ml_hostname;
    mnt3_dirpath ml_directory;
    mountlist ml_next;
};

int mnt_dump(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void mnt_dump_free(nfs_res_t *res);

extern bool xdr_mountbody_x(XDR *xdrs, mountbody *objp);

extern bool xdr_mountlist(XDR *, mountlist *);


#endif //DNFSD_MNT_DUMP_H
