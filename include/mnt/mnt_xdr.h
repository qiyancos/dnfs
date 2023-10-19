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
#ifndef DNFSD_MNT_XDR_H
#define DNFSD_MNT_XDR_H
extern "C" {
#include "rpc/xdr_inline.h"
}
#include <cassert>
#include "dnfsd/dnfs_meta_data.h"
bool xdr_mountstat3(XDR *, mountstat3 *);
bool xdr_fhandle3(XDR *, fhandle3 *);
bool xdr_dirpath(XDR *, mnt3_dirpath *);
bool xdr_name(XDR *, mnt3_name *);

#endif //DNFSD_MNT_XDR_H
