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
/*nfs构建数据*/
#ifndef NFS_NFS_FUNC_H
#define NFS_NFS_FUNC_H

#include "dnfsd/dnfs_meta_data.h"
#include "nfs/nfs_null.h"
#include "nfs/nfs_getattr.h"
#include "nfs/nfs_setattr.h"
#include "nfs/nfs_lookup.h"
#include "nfs/nfs_access.h"
#include "nfs/nfs_readlink.h"
#include "nfs/nfs_read.h"
#include "nfs/nfs_write.h"
#include "nfs/nfs_create.h"
#include "nfs/nfs_mkdir.h"
#include "nfs/nfs_symlink.h"
#include "nfs/nfs_mknod.h"
#include "nfs/nfs_remove.h"
#include "nfs/nfs_rmdir.h"
#include "nfs/nfs_rename.h"
#include "nfs/nfs_link.h"
#include "nfs/nfs_readdir.h"
#include "nfs/nfs_readdirplus.h"
#include "nfs/nfs_fsstat.h"
#include "nfs/nfs_fsinfo.h"
#include "nfs/nfs_pathconf.h"
#include "nfs/nfs_commit.h"

#define    NFS_PROGRAM    100003
#define    NFS_V3 3
#define    NFS_PORT 2049

#define    NFSPROC_NULL    0
#define    NFSPROC_GETATTR    1
#define    NFSPROC_SETATTR    2
#define    NFSPROC_ROOT    3
#define    NFSPROC_LOOKUP    4
#define    NFSPROC_READLINK    5
#define    NFSPROC_READ    6
#define    NFSPROC_WRITECACHE    7
#define    NFSPROC_WRITE    8
#define    NFSPROC_CREATE    9
#define    NFSPROC_REMOVE    10
#define    NFSPROC_RENAME    11
#define    NFSPROC_LINK    12
#define    NFSPROC_SYMLINK    13
#define    NFSPROC_MKDIR    14
#define    NFSPROC_RMDIR    15
#define    NFSPROC_READDIR    16
#define    NFSPROC_STATFS    17
#define    NFSPROC3_NULL    0
#define    NFSPROC3_GETATTR    1
#define    NFSPROC3_SETATTR    2
#define    NFSPROC3_LOOKUP    3
#define    NFSPROC3_ACCESS    4
#define    NFSPROC3_READLINK    5
#define    NFSPROC3_READ    6
#define    NFSPROC3_WRITE    7
#define    NFSPROC3_CREATE    8
#define    NFSPROC3_MKDIR    9
#define    NFSPROC3_SYMLINK    10
#define    NFSPROC3_MKNOD    11
#define    NFSPROC3_REMOVE    12
#define    NFSPROC3_RMDIR    13
#define    NFSPROC3_RENAME    14
#define    NFSPROC3_LINK    15
#define    NFSPROC3_READDIR    16
#define    NFSPROC3_READDIRPLUS    17
#define    NFSPROC3_FSSTAT    18
#define    NFSPROC3_FSINFO    19
#define    NFSPROC3_PATHCONF    20
#define    NFSPROC3_COMMIT    21

#define NOTHING_SPECIAL 0x0000    /* Nothing to be done for this kind of
				   request */
#define MAKES_WRITE    0x0001    /* The function modifyes the FSAL
				   (not permitted for RO FS) */
#define NEEDS_CRED    0x0002    /* A credential is needed for this
				   operation */
#define CAN_BE_DUP    0x0004    /* Handling of dup request can be done
				   for this request */
#define SUPPORTS_GSS    0x0008    /* Request may be authenticated by
				   RPCSEC_GSS */
#define MAKES_IO    0x0010    /* Request may do I/O
				   (not allowed on MD ONLY exports */
#define FSF3_LINK 0x0001
#define FSF3_SYMLINK 0x0002
#define FSF3_HOMOGENEOUS 0x0008
#define FSF3_CANSETTIME 0x0010

/* NFS4，RPC相关函数的处理描述信息，包括函数、xdr函数、函数名等等 */
const nfs_function_desc_t nfs3_func_desc[] = {
        {
                .service_function = nfs_null,
                .free_function = nfs_null_free,
                .xdr_decode_func = (xdrproc_t) xdr_void,
                .xdr_encode_func = (xdrproc_t) xdr_void,
                .funcname = "NFS3_NULL",
                .dispatch_behaviour = NOTHING_SPECIAL},
        {
                .service_function = nfs3_getattr,
                .free_function = nfs3_getattr_free,
                .xdr_decode_func = (xdrproc_t) xdr_GETATTR3args,
                .xdr_encode_func = (xdrproc_t) xdr_GETATTR3res,
                .funcname = "NFS3_GETATTR",
                .dispatch_behaviour = NEEDS_CRED | SUPPORTS_GSS},
        {
                .service_function = nfs3_setattr,
                .free_function = nfs3_setattr_free,
                .xdr_decode_func = (xdrproc_t) xdr_SETATTR3args,
                .xdr_encode_func = (xdrproc_t) xdr_SETATTR3res,
                .funcname = "NFS3_SETATTR",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_lookup,
                .free_function = nfs3_lookup_free,
                .xdr_decode_func = (xdrproc_t) xdr_LOOKUP3args,
                .xdr_encode_func = (xdrproc_t) xdr_LOOKUP3res,
                .funcname = "NFS3_LOOKUP",
                .dispatch_behaviour = NEEDS_CRED | SUPPORTS_GSS},
        {
                .service_function = nfs3_access,
                .free_function = nfs3_access_free,
                .xdr_decode_func = (xdrproc_t) xdr_ACCESS3args,
                .xdr_encode_func = (xdrproc_t) xdr_ACCESS3res,
                .funcname = "NFS3_ACCESS",
                .dispatch_behaviour = NEEDS_CRED | SUPPORTS_GSS},
        {
                .service_function = nfs3_readlink,
                .free_function = nfs3_readlink_free,
                .xdr_decode_func = (xdrproc_t) xdr_READLINK3args,
                .xdr_encode_func = (xdrproc_t) xdr_READLINK3res,
                .funcname = "NFS3_READLINK",
                .dispatch_behaviour = NEEDS_CRED | SUPPORTS_GSS},
        {
                .service_function = nfs3_read,
                .free_function = nfs3_read_free,
                .xdr_decode_func = (xdrproc_t) xdr_READ3args,
                .xdr_encode_func = (xdrproc_t) xdr_READ3res,
                .funcname = "NFS3_READ",
                .dispatch_behaviour =
                NEEDS_CRED | SUPPORTS_GSS | MAKES_IO},
        {
                .service_function = nfs3_write,
                .free_function = nfs3_write_free,
                .xdr_decode_func = (xdrproc_t) xdr_WRITE3args,
                .xdr_encode_func = (xdrproc_t) xdr_WRITE3res,
                .funcname = "NFS3_WRITE",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS |
                 MAKES_IO)
        },
        {
                .service_function = nfs3_create,
                .free_function = nfs3_create_free,
                .xdr_decode_func = (xdrproc_t) xdr_CREATE3args,
                .xdr_encode_func = (xdrproc_t) xdr_CREATE3res,
                .funcname = "NFS3_CREATE",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_mkdir,
                .free_function = nfs3_mkdir_free,
                .xdr_decode_func = (xdrproc_t) xdr_MKDIR3args,
                .xdr_encode_func = (xdrproc_t) xdr_MKDIR3res,
                .funcname = "NFS3_MKDIR",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_symlink,
                .free_function = nfs3_symlink_free,
                .xdr_decode_func = (xdrproc_t) xdr_SYMLINK3args,
                .xdr_encode_func = (xdrproc_t) xdr_SYMLINK3res,
                .funcname = "NFS3_SYMLINK",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_mknod,
                .free_function = nfs3_mknod_free,
                .xdr_decode_func = (xdrproc_t) xdr_MKNOD3args,
                .xdr_encode_func = (xdrproc_t) xdr_MKNOD3res,
                .funcname = "NFS3_MKNOD",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_remove,
                .free_function = nfs3_remove_free,
                .xdr_decode_func = (xdrproc_t) xdr_REMOVE3args,
                .xdr_encode_func = (xdrproc_t) xdr_REMOVE3res,
                .funcname = "NFS3_REMOVE",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_rmdir,
                .free_function = nfs3_rmdir_free,
                .xdr_decode_func = (xdrproc_t) xdr_RMDIR3args,
                .xdr_encode_func = (xdrproc_t) xdr_RMDIR3res,
                .funcname = "NFS3_RMDIR",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_rename,
                .free_function = nfs3_rename_free,
                .xdr_decode_func = (xdrproc_t) xdr_RENAME3args,
                .xdr_encode_func = (xdrproc_t) xdr_RENAME3res,
                .funcname = "NFS3_RENAME",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_link,
                .free_function = nfs3_link_free,
                .xdr_decode_func = (xdrproc_t) xdr_LINK3args,
                .xdr_encode_func = (xdrproc_t) xdr_LINK3res,
                .funcname = "NFS3_LINK",
                .dispatch_behaviour =
                (MAKES_WRITE | NEEDS_CRED | CAN_BE_DUP | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_readdir,
                .free_function = nfs3_readdir_free,
                .xdr_decode_func = (xdrproc_t) xdr_READDIR3args,
                .xdr_encode_func = (xdrproc_t) xdr_READDIR3res,
                .funcname = "NFS3_READDIR",
                .dispatch_behaviour = (NEEDS_CRED | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_readdirplus,
                .free_function = nfs3_readdirplus_free,
                .xdr_decode_func = (xdrproc_t) xdr_READDIRPLUS3args,
                .xdr_encode_func = (xdrproc_t) xdr_READDIRPLUS3res,
                .funcname = "NFS3_READDIRPLUS",
                .dispatch_behaviour = (NEEDS_CRED | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_fsstat,
                .free_function = nfs3_fsstat_free,
                .xdr_decode_func = (xdrproc_t) xdr_FSSTAT3args,
                .xdr_encode_func = (xdrproc_t) xdr_FSSTAT3res,
                .funcname = "NFS3_FSSTAT",
                .dispatch_behaviour = (NEEDS_CRED | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_fsinfo,
                .free_function = nfs3_fsinfo_free,
                .xdr_decode_func = (xdrproc_t) xdr_FSINFO3args,
                .xdr_encode_func = (xdrproc_t) xdr_FSINFO3res,
                .funcname = "NFS3_FSINFO",
                .dispatch_behaviour = (NEEDS_CRED)
        },
        {
                .service_function = nfs3_pathconf,
                .free_function = nfs3_pathconf_free,
                .xdr_decode_func = (xdrproc_t) xdr_PATHCONF3args,
                .xdr_encode_func = (xdrproc_t) xdr_PATHCONF3res,
                .funcname = "NFS3_PATHCONF",
                .dispatch_behaviour = (NEEDS_CRED | SUPPORTS_GSS)
        },
        {
                .service_function = nfs3_commit,
                .free_function = nfs3_commit_free,
                .xdr_decode_func = (xdrproc_t) xdr_COMMIT3args,
                .xdr_encode_func = (xdrproc_t) xdr_COMMIT3res,
                .funcname = "NFS3_COMMIT",
                .dispatch_behaviour = (MAKES_WRITE | NEEDS_CRED | SUPPORTS_GSS)}
};

#endif //NFS_NFS_FUNC_H
