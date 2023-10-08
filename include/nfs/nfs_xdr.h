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

#ifndef DNFSD_NFS_XDR_H
#define DNFSD_NFS_XDR_H
extern "C" {
#include "rpc/xdr_inline.h"
#include "rpc/xdr.h"
}
#include "nfs/nfs_args.h"

extern bool xdr_nfs3_uint32(XDR *xdrs, nfs3_uint32 *objp);
extern bool xdr_size3(XDR *xdrs, size3 *objp);
extern bool xdr_nfs3_uint64(XDR *xdrs, nfs3_uint64 *objp);
extern bool xdr_nfstime3(XDR *xdrs, nfstime3 *objp);
extern bool xdr_nfsstat3(XDR *xdrs, nfsstat3 *objp);
extern bool xdr_ftype3(XDR *xdrs, ftype3 *objp);
extern bool xdr_uid3(XDR *xdrs, uid3 *objp);
extern bool xdr_gid3(XDR *xdrs, gid3 *objp);
extern bool xdr_specdata3(XDR *xdrs, specdata3 *objp);
extern bool xdr_fileid3(XDR *xdrs, fileid3 *objp);
extern bool xdr_fattr3(XDR *xdrs, fattr3 *objp);
extern bool xdr_post_op_attr(XDR *xdrs, post_op_attr *objp);
extern bool xdr_nfs_fh3(XDR *xdrs, nfs_fh3 *objp);
extern bool xdr_filename3(XDR *xdrs, filename3 *objp);
extern bool xdr_diropargs3(XDR *xdrs, diropargs3 *objp);
extern bool xdr_wcc_attr(XDR *xdrs, wcc_attr *objp);
extern bool xdr_pre_op_attr(XDR *xdrs, pre_op_attr *objp);
extern bool xdr_wcc_data(XDR *xdrs, wcc_data *objp);
extern bool xdr_cookie3(XDR *xdrs, cookie3 *objp);
extern bool xdr_cookieverf3(XDR *xdrs, cookieverf3 objp);
extern bool xdr_count3(XDR *xdrs, count3 *objp);
extern bool xdr_post_op_fh3(XDR *xdrs, post_op_fh3 *objp);
#endif //DNFSD_NFS_XDR_H
