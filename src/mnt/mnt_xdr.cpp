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