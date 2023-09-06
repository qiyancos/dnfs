/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
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

#include "log/log.h"
#include "nfs/nfs_compound_data.h"

static inline bool xdr_COMPOUND4args(XDR *xdrs, COMPOUND4args *objp) {
    if (!xdr_utf8str_cs(xdrs, &objp->tag))
        return false;
    if (!inline_xdr_u_int32_t(xdrs, &objp->minorversion))
        return false;
    /* decoder hint */
    if (objp->minorversion > 0)
        xdrs->x_flags &= ~XDR_FLAG_CKSUM;
    if (!xdr_array(xdrs,
                   (char **)&objp->argarray.argarray_val,
                   &objp->argarray.argarray_len, XDR_ARRAY_MAXLEN,
                   sizeof(nfs_argop4), (xdrproc_t) xdr_nfs_argop4))
        return false;
    return true;
}

bool xdr_COMPOUND4res_extended(XDR *xdrs, struct COMPOUND4res_extended **objp) {
    /* Since the response in nfs_res_t is a pointer, we must dereference it
     * to complete the encode.
     */
    struct COMPOUND4res_extended *res_compound4_extended = *objp;

    /* And we must pass the actual COMPOUND4res */
    return xdr_COMPOUND4res(xdrs, &res_compound4_extended->res_compound4);
}