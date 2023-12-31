/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
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

#ifndef DNFSD_NFS_XDR_H
#define DNFSD_NFS_XDR_H
extern "C" {
#include "rpc/xdr_inline.h"
#include "rpc/xdr.h"
}

#include "nfs/nfs_args.h"

bool xdr_nfs3_uint32(XDR *xdrs, nfs3_uint32 *objp);

bool xdr_size3(XDR *xdrs, size3 *objp);

bool xdr_nfs3_uint64(XDR *xdrs, nfs3_uint64 *objp);

bool xdr_nfstime3(XDR *xdrs, nfstime3 *objp);

bool xdr_nfsstat3(XDR *xdrs, nfsstat3 *objp);

bool xdr_ftype3(XDR *xdrs, ftype3 *objp);

bool xdr_uid3(XDR *xdrs, uid3 *objp);

bool xdr_gid3(XDR *xdrs, gid3 *objp);

bool xdr_specdata3(XDR *xdrs, specdata3 *objp);

bool xdr_fileid3(XDR *xdrs, fileid3 *objp);

bool xdr_fattr3(XDR *xdrs, fattr3 *objp);

bool xdr_post_op_attr(XDR *xdrs, post_op_attr *objp);

bool xdr_nfs_fh3(XDR *xdrs, nfs_fh3 *objp);

bool xdr_filename3(XDR *xdrs, filename3 *objp);

bool xdr_diropargs3(XDR *xdrs, diropargs3 *objp);

bool xdr_wcc_attr(XDR *xdrs, wcc_attr *objp);

bool xdr_pre_op_attr(XDR *xdrs, pre_op_attr *objp);

bool xdr_wcc_data(XDR *xdrs, wcc_data *objp);

bool xdr_cookie3(XDR *xdrs, cookie3 *objp);

bool xdr_cookieverf3(XDR *xdrs, cookieverf3 objp);

bool xdr_count3(XDR *xdrs, count3 *objp);

bool xdr_post_op_fh3(XDR *xdrs, post_op_fh3 *objp);

bool xdr_set_mode3(XDR *xdrs, set_mode3 *objp);

bool xdr_set_uid3(XDR *xdrs, set_uid3 *objp);

bool xdr_set_gid3(XDR *xdrs, set_gid3 *objp);

bool xdr_set_size3(XDR *xdrs, set_size3 *objp);

bool xdr_time_how(XDR *xdrs, time_how *objp);

bool xdr_set_atime(XDR *xdrs, set_atime *objp);

bool xdr_set_mtime(XDR *xdrs, set_mtime *objp);

bool xdr_sattr3(XDR *xdrs, sattr3 *objp);

bool xdr_nfspath3(XDR *xdrs, nfspath3 *objp);

bool xdr_writeverf3(XDR *xdrs, writeverf3 objp);

bool xdr_offset3(XDR *xdrs, offset3 *objp);

#endif //DNFSD_NFS_XDR_H
