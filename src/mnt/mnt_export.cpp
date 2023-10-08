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
#include "mnt/mnt_xdr.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "MNT"

int mnt_export(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    LOG(MODULE_NAME, D_INFO,"REQUEST PROCESSING: Calling MNT_EXPORT");

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

bool xdr_groupnode_x(XDR *xdrs, groupnode *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!xdr_name(xdrs, &objp->gr_name))
        return (false);
    return (true);
}

bool xdr_groups(XDR *xdrs, struct groupnode **objp) {
    /*
     * more_elements is pre-computed in case the direction is
     * XDR_ENCODE or XDR_FREE.  more_elements is overwritten by
     * xdr_bool when the direction is XDR_DECODE.
     */
    int freeing;
    struct groupnode **next = nullptr;    /* pacify gcc */
    bool_t more_elements = false;    /* yes, bool_t */

    assert(xdrs != nullptr);
    assert(objp != nullptr);

    freeing = (xdrs->x_op == XDR_FREE);

    for (;;) {
        more_elements = (bool_t) (*objp != nullptr);
        if (!xdr_bool(xdrs, &more_elements))
            return (false);
        if (!more_elements)
            return (true);    /* we are done */
        /*
         * the unfortunate side effect of non-recursion is that in
         * the case of freeing we must remember the next object
         * before we free the current object ...
         */
        if (freeing)
            next = &((*objp)->gr_next);
        if (!xdr_reference(xdrs, (void **) objp,
                           (u_int) sizeof(struct groupnode),
                           (xdrproc_t) xdr_groupnode_x))
            return (false);
        objp = (freeing) ? next : &((*objp)->gr_next);
    }
}

bool xdr_exportnode_x(XDR *xdrs, exportnode *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!xdr_dirpath(xdrs, &objp->ex_dir))
        return (false);
    if (!xdr_groups(xdrs, &objp->ex_groups))
        return (false);
    return (true);
}

bool xdr_exports(XDR *xdrs, struct exportnode **objp) {
    /*
     * more_elements is pre-computed in case the direction is
     * XDR_ENCODE or XDR_FREE.  more_elements is overwritten by
     * xdr_bool when the direction is XDR_DECODE.
     */
    int freeing;
    struct exportnode **next = nullptr;    /* pacify gcc */
    bool_t more_elements = false;        /* yes, bool_t */

    assert(xdrs != nullptr);
    assert(objp != nullptr);

    freeing = (xdrs->x_op == XDR_FREE);

    for (;;) {
        more_elements = (bool_t) (*objp != nullptr);
        if (!xdr_bool(xdrs, &more_elements))
            return (false);
        if (!more_elements)
            return (true);    /* we are done */
        /*
         * the unfortunate side effect of non-recursion is that in
         * the case of freeing we must remember the next object
         * before we free the current object ...
         */
        if (freeing)
            next = &((*objp)->ex_next);
        if (!xdr_reference(xdrs, (void **) objp,
                           (u_int) sizeof(struct exportnode),
                           (xdrproc_t) xdr_exportnode_x))
            return (false);
        objp = (freeing) ? next : &((*objp)->ex_next);
    }
}
