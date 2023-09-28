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
#include "mnt/mnt_export.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "MNT"

int mnt_export(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    res->res_mntexport->ex_dir=(char*)"/export_test";
    res->res_mntexport->ex_groups= nullptr;
    res->res_mntexport->ex_next= nullptr;
    return NFS_REQ_OK;
}

void mnt_export_free(nfs_res_t *res) {
    struct exportnode *exp, *next_exp;
    struct groupnode *grp, *next_grp;

    exp = res->res_mntexport;
    while (exp != nullptr) {
        next_exp = exp->ex_next;
        grp = exp->ex_groups;
        while (grp != nullptr) {
            next_grp = grp->gr_next;
            if (grp->gr_name != nullptr)
                free(grp->gr_name);
            free(grp);
            grp = next_grp;
        }
        free(exp);
        exp = next_exp;
    }
}
