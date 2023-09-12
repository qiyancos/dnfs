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
#include "rpc/xdr.h"
}

/* 通用XDR处理函数 */
static inline bool xdr_COMPOUND4args(XDR *xdrs, COMPOUND4args *objp);

bool xdr_COMPOUND4res_extended(XDR *xdrs, struct COMPOUND4res_extended **objp);

#endif //DNFSD_NFS_XDR_H