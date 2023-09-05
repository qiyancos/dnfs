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

#ifndef NFS_NFS_FUNC_H
#define NFS_NFS_FUNC_H

extern "C" {
#include "rpc/xdr.h"
}

#include "nfs/nfs_args.h"
#include "nfs/nfs_res.h"

enum nfs_req_result {
    NFS_REQ_OK,
    NFS_REQ_DROP,
    NFS_REQ_ERROR,
    NFS_REQ_REPLAY,
    NFS_REQ_ASYNC_WAIT,
    NFS_REQ_XPRT_DIED,
    NFS_REQ_AUTH_ERR,
};

#define NOTHING_SPECIAL 0x0000	/* Nothing to be done for this kind of
				   request */
#define MAKES_WRITE	0x0001	/* The function modifyes the FSAL
				   (not permitted for RO FS) */
#define NEEDS_CRED	0x0002	/* A credential is needed for this
				   operation */
#define CAN_BE_DUP	0x0004	/* Handling of dup request can be done
				   for this request */
#define SUPPORTS_GSS	0x0008	/* Request may be authenticated by
				   RPCSEC_GSS */
#define MAKES_IO	0x0010	/* Request may do I/O
				   (not allowed on MD ONLY exports */

/* 标准RPC处理函数类别 */
typedef int (*nfs_protocol_function_t) (nfs_arg_t *,
                                        struct svc_req *,
                                        nfs_res_t *);

/* 标准RPC函数后处理释放空间处理类别 */
typedef void (*nfs_protocol_free_t) (nfs_res_t *);

/* RPC函数的单个函数描述符结构体 */
typedef struct nfs_function_desc__ {
    nfs_protocol_function_t service_function;
    nfs_protocol_free_t free_function;
    xdrproc_t xdr_decode_func;
    xdrproc_t xdr_encode_func;
    const char *funcname;
    unsigned int dispatch_behaviour;
} nfs_function_desc_t;

/* 无效操作函数的相应处理类别 */
const nfs_function_desc_t invalid_funcdesc = {
        .service_function = NULL,
        .free_function = NULL,
        .xdr_decode_func = (xdrproc_t) xdr_void,
        .xdr_encode_func = (xdrproc_t) xdr_void,
        .funcname = "invalid_function",
        .dispatch_behaviour = NOTHING_SPECIAL
};

/* NFS NULL Process function*/
int nfs_null([[maybe_unused]] nfs_arg_t *arg,
             [[maybe_unused]] struct svc_req *req,
             [[maybe_unused]] nfs_res_t *res);

/* NFS FREE Process Function */
void nfs_null_free([[maybe_unused]] nfs_res_t *res);

/* NFS COUNPOUND Process Function */
int nfs4_Compound(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

/* NFS COUNPOUND FREE Process Function */
void nfs4_Compound_Free(nfs_res_t *res);

/* NFS4，RPC相关函数的处理描述信息，包括函数、xdr函数、函数名等等 */
const nfs_function_desc_t nfs4_func_desc[] = {
    {
        .service_function = nfs_null,
        .free_function = nfs_null_free,
        .xdr_decode_func = (xdrproc_t) xdr_void,
        .xdr_encode_func = (xdrproc_t) xdr_void,
        .funcname = "NFS_NULL",
        .dispatch_behaviour = NOTHING_SPECIAL
    },{
        .service_function = nfs4_Compound,
        .free_function = nfs4_Compound_Free,
        .xdr_decode_func = (xdrproc_t) xdr_COMPOUND4args,
        .xdr_encode_func = (xdrproc_t) xdr_COMPOUND4res_extended,
        .funcname = "NFS4_COMP",
        .dispatch_behaviour = CAN_BE_DUP
    }
};

/* 通用XDR处理函数 */

/* 通用的NFS处理函数 */

#endif //NFS_NFS_FUNC_H
