/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#include "mnt/mnt_dump.h"
#include "mnt/mnt_xdr.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "MNT"

int mnt_dump(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    LOG(MODULE_NAME, D_INFO, "REQUEST PROCESSING: Calling MNT_DUMP");

    res->res_dump = nullptr;

    return NFS_REQ_OK;
}

void mnt_dump_free(nfs_res_t *res) {
    /* Nothing to do */
}

bool xdr_mountbody_x(XDR *xdrs, mountbody *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!xdr_name(xdrs, &objp->ml_hostname))
        return (false);
    if (!xdr_dirpath(xdrs, &objp->ml_directory))
        return (false);
    return (true);
}

bool xdr_mountlist(XDR *xdrs, struct mountbody **objp) {
    /*
     * more_elements is pre-computed in case the direction is
     * XDR_ENCODE or XDR_FREE.  more_elements is overwritten by
     * xdr_bool when the direction is XDR_DECODE.
     */
    int freeing;
    struct mountbody **next = nullptr;    /* pacify gcc */
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
            next = &((*objp)->ml_next);
        if (!xdr_reference(xdrs, (void **) objp,
                           (u_int) sizeof(struct mountbody),
                           (xdrproc_t) xdr_mountbody_x))
            return (false);
        objp = (freeing) ? next : &((*objp)->ml_next);
    }
}