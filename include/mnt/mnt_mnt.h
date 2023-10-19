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
#ifndef DNFSD_MNT_MNT_H
#define DNFSD_MNT_MNT_H

#include "mnt_args.h"

typedef struct {
    u_int fhandle3_len;
    char *fhandle3_val;
} fhandle3;

struct mountres3_ok {
    fhandle3 fhandle;
    struct {
        u_int auth_flavors_len;
        int *auth_flavors_val;
    } auth_flavors;
};

struct mountres3 {
    mountstat3 fhs_status;
    union {
        mountres3_ok mountinfo;
    } mountres3_u;
};

int mnt_mnt(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void mnt3_mnt_free(nfs_res_t *res);

bool xdr_mountres3_ok(XDR *, mountres3_ok *);

bool xdr_mountres3(XDR *, mountres3 *);

#endif //DNFSD_MNT_MNT_H
