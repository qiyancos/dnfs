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
#include "dnfsd/dnfs_meta_data.h"
extern bool xdr_mountstat3(XDR *, mountstat3 *);
extern bool xdr_fhandle3(XDR *, fhandle3 *);
extern bool xdr_dirpath(XDR *, mnt3_dirpath *);
extern bool xdr_name(XDR *, mnt3_name *);
extern bool xdr_mountres3_ok(XDR *, mountres3_ok *);
extern bool xdr_mountres3(XDR *, mountres3 *);
extern bool xdr_mountlist(XDR *, mountlist *);
extern bool xdr_groups(XDR *, mnt3_groups *);
extern bool xdr_exports(XDR *, mnt3_exports *);

#endif //DNFSD_MNT_XDR_H
