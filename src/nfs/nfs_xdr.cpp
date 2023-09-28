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
#include "rpc/xdr.h"
}

#include "nfs/nfs_xdr.h"
#include "log/log.h"

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
xdr_mode3(XDR *xdrs, mode3 *objp)
{
	if (!xdr_nfs3_uint32(xdrs, objp))
		return (false);
	return (true);
}

bool
xdr_fattr3(XDR *xdrs, fattr3 *objp)
{
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


bool xdr_FSINFO3args(XDR *xdrs, FSINFO3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->fsroot))
        return (false);
    return (true);
}

bool xdr_FSINFO3resok(XDR *xdrs, FSINFO3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtmax))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtpref))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtmult))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtmax))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtpref))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtmult))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->dtpref))
        return (false);
    if (!xdr_size3(xdrs, &objp->maxfilesize))
        return (false);
    if (!xdr_nfstime3(xdrs, &objp->time_delta))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->properties))
        return (false);
    return (true);
}

bool xdr_FSINFO3resfail(XDR *xdrs, FSINFO3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_FSINFO3res(XDR *xdrs, FSINFO3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_FSINFO3resok(xdrs, &objp->FSINFO3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_FSINFO3resfail(xdrs, &objp->FSINFO3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}

bool xdr_PATHCONF3args(XDR *xdrs, PATHCONF3args *objp)
{
    if (!xdr_nfs_fh3(xdrs, &objp->object))
        return (false);
    return (true);
}

bool xdr_PATHCONF3resok(XDR *xdrs, PATHCONF3resok *objp)
{
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->linkmax))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->name_max))
        return (false);
    if (!xdr_bool(xdrs, &objp->no_trunc))
        return (false);
    if (!xdr_bool(xdrs, &objp->chown_restricted))
        return (false);
    if (!xdr_bool(xdrs, &objp->case_insensitive))
        return (false);
    if (!xdr_bool(xdrs, &objp->case_preserving))
        return (false);
    return (true);
}

bool xdr_PATHCONF3resfail(XDR *xdrs, PATHCONF3resfail *objp)
{
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_PATHCONF3res(XDR *xdrs, PATHCONF3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_PATHCONF3resok(xdrs, &objp->PATHCONF3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_PATHCONF3resfail(xdrs, &objp->PATHCONF3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}

bool xdr_GETATTR3args(XDR *xdrs, GETATTR3args *objp)
{
    if (!xdr_nfs_fh3(xdrs, &objp->object))
        return (false);
    return (true);
}

bool xdr_GETATTR3resok(XDR *xdrs, GETATTR3resok *objp)
{
    if (!xdr_fattr3(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_GETATTR3res(XDR *xdrs, GETATTR3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_GETATTR3resok(xdrs, &objp->GETATTR3res_u.resok))
                return (false);
            break;
        default:
            return (true);
            break;
    }
    return (true);
}