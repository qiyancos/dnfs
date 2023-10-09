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

#include "nfs/nfs_xdr.h"

#define MODULE_NAME "NFS"

bool xdr_nfs3_uint32(XDR *xdrs, nfs3_uint32 *objp) {
    if (!xdr_u_int(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_nfs3_uint64(XDR *xdrs, nfs3_uint64 *objp) {
    if (!xdr_u_longlong_t(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_size3(XDR *xdrs, size3 *objp) {
    if (!xdr_nfs3_uint64(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_nfstime3(XDR *xdrs, nfstime3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, &objp->tv_sec))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->tv_nsec))
        return (false);
    return (true);
}

bool xdr_nfsstat3(XDR *xdrs, nfsstat3 *objp) {
    if (!xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_ftype3(XDR *xdrs, ftype3 *objp) {
    if (!xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_uid3(XDR *xdrs, uid3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_gid3(XDR *xdrs, gid3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_specdata3(XDR *xdrs, specdata3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, &objp->specdata1))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->specdata2))
        return (false);
    return (true);
}

bool xdr_fileid3(XDR *xdrs, fileid3 *objp) {
    if (!xdr_nfs3_uint64(xdrs, objp))
        return (false);
    return (true);
}

bool
xdr_mode3(XDR *xdrs, mode3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, objp))
        return (false);
    return (true);
}

bool
xdr_fattr3(XDR *xdrs, fattr3 *objp) {
    if (!xdr_ftype3(xdrs, &objp->type))
        return (false);
    if (!xdr_mode3(xdrs, &objp->mode))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->nlink))
        return (false);
    if (!xdr_uid3(xdrs, &objp->uid))
        return (false);
    if (!xdr_gid3(xdrs, &objp->gid))
        return (false);
    if (!xdr_size3(xdrs, &objp->size))
        return (false);
    if (!xdr_size3(xdrs, &objp->used))
        return (false);
    if (!xdr_specdata3(xdrs, &objp->rdev))
        return (false);
    if (!xdr_nfs3_uint64(xdrs, &objp->fsid))
        return (false);
    if (!xdr_fileid3(xdrs, &objp->fileid))
        return (false);
    if (!xdr_nfstime3(xdrs, &objp->atime))
        return (false);
    if (!xdr_nfstime3(xdrs, &objp->mtime))
        return (false);
    if (!xdr_nfstime3(xdrs, &objp->ctime))
        return (false);
    return (true);
}

bool xdr_post_op_attr(XDR *xdrs, post_op_attr *objp) {
    if (!xdr_bool(xdrs, &objp->attributes_follow))
        return (false);
    switch (objp->attributes_follow) {
        case TRUE:
            if (!xdr_fattr3(xdrs, &objp->post_op_attr_u.attributes))
                return (false);
            break;
        case FALSE:
            break;
        default:
            return (false);
    }
    return (true);
}

bool xdr_nfs_fh3(XDR *xdrs, nfs_fh3 *objp) {
    if (!xdr_bytes
            (xdrs, (char **) &objp->data.data_val,
             (u_int *) &objp->data.data_len, 64))
        return (false);
    return (true);
}


bool xdr_cookie3(XDR *xdrs, cookie3 *objp) {
    if (!xdr_nfs3_uint64(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_cookieverf3(XDR *xdrs, cookieverf3 objp) {
    if (!xdr_opaque(xdrs, objp, 8))
        return (false);
    return (true);
}

bool xdr_count3(XDR *xdrs, count3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_filename3(XDR *xdrs, filename3 *objp) {
    if (!xdr_string(xdrs, objp, XDR_STRING_MAXLEN))
        return (false);
    return (true);
}

bool xdr_post_op_fh3(XDR *xdrs, post_op_fh3 *objp) {
    if (!xdr_bool(xdrs, &objp->handle_follows))
        return (false);
    switch (objp->handle_follows) {
        case TRUE:
            if (!xdr_nfs_fh3(xdrs, &objp->post_op_fh3_u.handle))
                return (false);
            break;
        case FALSE:
            break;
        default:
            return (false);
    }
    return (true);
}

bool xdr_diropargs3(XDR *xdrs, diropargs3 *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->dir))
        return (false);
    if (!xdr_filename3(xdrs, &objp->name))
        return (false);
    return (true);
}

bool xdr_wcc_attr(XDR *xdrs, wcc_attr *objp) {
    if (!xdr_size3(xdrs, &objp->size))
        return (false);
    if (!xdr_nfstime3(xdrs, &objp->mtime))
        return (false);
    if (!xdr_nfstime3(xdrs, &objp->ctime))
        return (false);
    return (true);
}

bool xdr_pre_op_attr(XDR *xdrs, pre_op_attr *objp) {
    if (!xdr_bool(xdrs, &objp->attributes_follow))
        return (false);
    switch (objp->attributes_follow) {
        case TRUE:
            if (!xdr_wcc_attr(xdrs, &objp->pre_op_attr_u.attributes))
                return (false);
            break;
        case FALSE:
            break;
        default:
            return (false);
    }
    return (true);
}

bool xdr_wcc_data(XDR *xdrs, wcc_data *objp) {
    if (!xdr_pre_op_attr(xdrs, &objp->before))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->after))
        return (false);
    return (true);
}

bool xdr_set_mode3(XDR *xdrs, set_mode3 *objp) {
    if (!xdr_bool(xdrs, &objp->set_it))
        return (false);
    switch (objp->set_it) {
        case TRUE:
            if (!xdr_mode3(xdrs, &objp->set_mode3_u.mode))
                return (false);
            break;
    }
    return (true);
}

bool xdr_set_uid3(XDR *xdrs, set_uid3 *objp) {
    if (!xdr_bool(xdrs, &objp->set_it))
        return (false);
    switch (objp->set_it) {
        case TRUE:
            if (!xdr_uid3(xdrs, &objp->set_uid3_u.uid))
                return (false);
            break;
    }
    return (true);
}

bool xdr_set_gid3(XDR *xdrs, set_gid3 *objp) {
    if (!xdr_bool(xdrs, &objp->set_it))
        return (false);
    switch (objp->set_it) {
        case TRUE:
            if (!xdr_gid3(xdrs, &objp->set_gid3_u.gid))
                return (false);
            break;
    }
    return (true);
}

bool xdr_set_size3(XDR *xdrs, set_size3 *objp) {
    if (!xdr_bool(xdrs, &objp->set_it))
        return (false);
    switch (objp->set_it) {
        case TRUE:
            if (!xdr_size3(xdrs, &objp->set_size3_u.size))
                return (false);
            break;
    }
    return (true);
}

bool xdr_time_how(XDR *xdrs, time_how *objp) {
    if (!xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_set_atime(XDR *xdrs, set_atime *objp) {
    if (!xdr_time_how(xdrs, &objp->set_it))
        return (false);
    switch (objp->set_it) {
        case SET_TO_CLIENT_TIME:
            if (!xdr_nfstime3(xdrs, &objp->set_atime_u.atime))
                return (false);
            break;
        default:
            return (true);
            break;
    }
    return (true);
}

bool xdr_set_mtime(XDR *xdrs, set_mtime *objp) {
    if (!xdr_time_how(xdrs, &objp->set_it))
        return (false);
    switch (objp->set_it) {
        case SET_TO_CLIENT_TIME:
            if (!xdr_nfstime3(xdrs, &objp->set_mtime_u.mtime))
                return (false);
            break;
        default:
            return (true);
            break;
    }
    return (true);
}

bool xdr_sattr3(XDR *xdrs, sattr3 *objp) {
    if (!xdr_set_mode3(xdrs, &objp->mode))
        return (false);
    if (!xdr_set_uid3(xdrs, &objp->uid))
        return (false);
    if (!xdr_set_gid3(xdrs, &objp->gid))
        return (false);
    if (!xdr_set_size3(xdrs, &objp->size))
        return (false);
    if (!xdr_set_atime(xdrs, &objp->atime))
        return (false);
    if (!xdr_set_mtime(xdrs, &objp->mtime))
        return (false);
    return (true);
}

bool xdr_nfspath3(XDR *xdrs, nfspath3 *objp) {
    if (!xdr_string(xdrs, objp, XDR_STRING_MAXLEN))
        return (false);
    return (true);
}

bool xdr_offset3(XDR *xdrs, offset3 *objp)
{
    if (!xdr_nfs3_uint64(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_writeverf3(XDR *xdrs, writeverf3 objp)
{
    if (!xdr_opaque(xdrs, objp, 8))
        return (false);
    return (true);
}