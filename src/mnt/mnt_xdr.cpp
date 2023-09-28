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
extern "C" {
#include "rpc/xdr_inline.h"
}

#include <cassert>
#include "mnt/mnt_xdr.h"
#include "dnfsd/dnfs_meta_data.h"

bool xdr_mountstat3(XDR *xdrs, mountstat3 *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!inline_xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_fhandle3(XDR *xdrs, fhandle3 *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif
    if (!inline_xdr_bytes
            (xdrs, (char **) &objp->fhandle3_val, (u_int *) &objp->fhandle3_len,
             NFS3_FHSIZE))
        return (false);

    return (true);
}

bool xdr_dirpath(XDR *xdrs, mnt3_dirpath *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!inline_xdr_string(xdrs, objp, MNTPATHLEN))
        return (false);
    return (true);
}

bool xdr_name(XDR *xdrs, mnt3_name *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!inline_xdr_string(xdrs, objp, MNTNAMLEN))
        return (false);
    return (true);
}

bool xdr_mountres3_ok(XDR *xdrs, mountres3_ok *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!xdr_fhandle3(xdrs, &objp->fhandle))
        return (false);
    if (!xdr_array
            (xdrs, (char **) &objp->auth_flavors.auth_flavors_val,
             &objp->auth_flavors.auth_flavors_len, XDR_ARRAY_MAXLEN,
             sizeof(int), (xdrproc_t) xdr_int))
        return (false);
    return (true);
}

bool xdr_mountres3(XDR *xdrs, mountres3 *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!xdr_mountstat3(xdrs, &objp->fhs_status))
        return (false);
    switch (objp->fhs_status) {
        case MNT3_OK:
            if (!xdr_mountres3_ok(xdrs, &objp->mountres3_u.mountinfo))
                return (false);
            break;
        default:
            return (true);
    }
    return (true);
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