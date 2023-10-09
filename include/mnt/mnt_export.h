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
#ifndef DNFSD_MNT_EXPORT_H
#define DNFSD_MNT_EXPORT_H

#include "mnt_args.h"

typedef struct groupnode *mnt3_groups;

struct groupnode {
    mnt3_name gr_name;
    mnt3_groups gr_next;
};

typedef struct exportnode *mnt3_exports;

struct exportnode {
    mnt3_dirpath ex_dir;
    mnt3_groups ex_groups;
    mnt3_exports ex_next;
};


struct proc_state {
    mnt3_exports head;
    mnt3_exports tail;
    int retval;
};


int mnt_export(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void mnt_export_free(nfs_res_t *res);

extern bool xdr_groupnode_x(XDR *xdrs, groupnode *objp);

extern bool xdr_groups(XDR *xdrs, struct groupnode **objp);

extern bool xdr_exportnode_x(XDR *xdrs, exportnode *objp);

extern bool xdr_exports(XDR *, mnt3_exports *);

#endif //DNFSD_MNT_EXPORT_H
