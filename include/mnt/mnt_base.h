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
/*mnt构建数据*/
#ifndef DNFSD_MNT_BASE_H
#define DNFSD_MNT_BASE_H
extern "C" {
#include "rpc/xdr.h"
};
#include "dnfsd/dnfs_meta_data.h"
#include "mnt_null.h"
#include "mnt_mnt.h"
#include "mnt_dump.h"
#include "mnt_umnt.h"
#include "mnt_umntAll.h"
#include "mnt_xdr.h"

#define    MNT_PROGRAM    100005
#define    MOUNT_V3 3
#define    MNT_PORT 0

#define    MOUNTPROC3_NULL    0
#define    MOUNTPROC3_MNT    1
#define    MOUNTPROC3_DUMP    2
#define    MOUNTPROC3_UMNT    3
#define    MOUNTPROC3_UMNTALL    4
#define    MOUNTPROC3_EXPORT    5

/* NFS4，RPC相关函数的处理描述信息，包括函数、xdr函数、函数名等等 */
const nfs_function_desc_t mnt3_func_desc[] = {
        {
                .service_function = mnt_null,
                .free_function = mnt_null_free,
                .xdr_decode_func = (xdrproc_t) xdr_void,
                .xdr_encode_func = (xdrproc_t) xdr_void,
                .funcname = "MNT_NULL",
                .dispatch_behaviour = NOTHING_SPECIAL},
        {
                .service_function = mnt_mnt,
                .free_function = mnt3_mnt_free,
                .xdr_decode_func = (xdrproc_t) xdr_dirpath,
                .xdr_encode_func = (xdrproc_t) xdr_mountres3,
                .funcname = "MNT_MNT",
                .dispatch_behaviour = NOTHING_SPECIAL},
        {
                .service_function = mnt_dump,
                .free_function = mnt_dump_free,
                .xdr_decode_func = (xdrproc_t) xdr_void,
                .xdr_encode_func = (xdrproc_t) xdr_mountlist,
                .funcname = "MNT_DUMP",
                .dispatch_behaviour = NOTHING_SPECIAL},
        {
                .service_function = mnt_umnt,
                .free_function = mnt_umnt_free,
                .xdr_decode_func = (xdrproc_t) xdr_dirpath,
                .xdr_encode_func = (xdrproc_t) xdr_void,
                .funcname = "MNT_UMNT",
                .dispatch_behaviour = NOTHING_SPECIAL},
        {
                .service_function = mnt_umntAll,
                .free_function = mnt_umntAll_free,
                .xdr_decode_func = (xdrproc_t) xdr_void,
                .xdr_encode_func = (xdrproc_t) xdr_void,
                .funcname = "MNT_UMNTALL",
                .dispatch_behaviour = NOTHING_SPECIAL},
        {
                .service_function = mnt_export,
                .free_function = mnt_export_free,
                .xdr_decode_func = (xdrproc_t) xdr_void,
                .xdr_encode_func = (xdrproc_t) xdr_exports,
                .funcname = "MNT_EXPORT",
                .dispatch_behaviour = NOTHING_SPECIAL}
};


#endif //DNFSD_MNT_BASE_H
