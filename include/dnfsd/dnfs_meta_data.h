/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 * Contributeur : Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
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

#ifndef DNFSD_DNFS_META_DATA_H
#define DNFSD_DNFS_META_DATA_H

extern "C" {
#include "rpc/svc.h"
}

#include <string>
#include "nfs/nfs_fsinfo.h"

struct dnfs_request_lookahead {
    uint32_t flags;
    uint16_t read;
    uint16_t write;
};

typedef struct nfs_core_param {
    /* 原始的绑定地址字符串 */
    std::string bind_addr_str = "0.0.0.0";
    /** The IPv4 address to which to bind for our
        listening port.  Set by the Bind_Addr option. */
    sockaddr_in bind_addr = {AF_INET, 0, {0}};

    /** Parameters affecting the relation with TIRPC.   */
    struct {
        /** Maximum number of connections for TIRPC.
            Defaults to 1024 and settable by
            RPC_Max_Connections. */
        uint32_t max_connections = 1024;
        /** Size of RPC send buffer.  Defaults to
            NFS_DEFAULT_SEND_BUFFER_SIZE and is settable by
            MaxRPCSendBufferSize.  */
        uint32_t max_send_buffer_size = 1048576;
        /** Size of RPC receive buffer.  Defaults to
            NFS_DEFAULT_RECV_BUFFER_SIZE and is settable by
            MaxRPCRecvBufferSize. */
        uint32_t max_recv_buffer_size = 1048576;
        /** Idle timeout (seconds).  Defaults to 5m */
        uint32_t idle_timeout_s = 300;
        /** TIRPC ioq min simultaneous io threads.  Defaults to
            2 and settable by rpc_ioq_thrdmin. */
        uint32_t ioq_thrd_min = 2;
        /** TIRPC ioq max simultaneous io threads.  Defaults to
            200 and settable by RPC_Ioq_ThrdMax. */
        uint32_t ioq_thrd_max = 200;
        struct {
            /** Partitions in GSS ctx cache table (default 13). */
            uint32_t ctx_hash_partitions = 13;
            /** Max GSS contexts in cache (i.e.,
             * max GSS clients, default 16K)
             */
            uint32_t max_ctx = 16384;
            /** Max entries to expire in one idle
             * check (default 200)
             */
            uint32_t max_gc = 200;
        } gss;
    } rpc;
    /** Whether tcp sockets should use SO_KEEPALIVE */
    bool enable_tcp_keepalive = true;
    /** Maximum number of TCP probes before dropping the connection */
    uint32_t tcp_keepcnt = 0;
    /** Idle time before TCP starts to send keepalive probes */
    uint32_t tcp_keepidle = 0;
    /** Time between each keepalive probe */
    uint32_t tcp_keepintvl = 0;
} nfs_core_parameter_t;

typedef struct nfs_param {
    /* NFS的核心参数数据，主要是针对NFSV23协议的内容 */
    nfs_core_parameter_t core_param;
} nfs_parameter_t;

/*请求参数*/
union nfs_arg_t {
    FSINFO3args arg_fsinfo3;
};

/*结果参数*/
union nfs_res_t {
    FSINFO3res res_fsinfo3;
};


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

/* 标准RPC处理函数类别 */
typedef int (*nfs_protocol_function_t)(nfs_arg_t *,
                                       struct svc_req *,
                                       nfs_res_t *);

/* 标准RPC函数后处理释放空间处理类别 */
typedef void (*nfs_protocol_free_t)(nfs_res_t *);

/* RPC函数的单个函数描述符结构体 */
typedef struct nfs_function_desc_ {
    nfs_protocol_function_t service_function;
    nfs_protocol_free_t free_function;
    xdrproc_t xdr_decode_func;
    xdrproc_t xdr_encode_func;
    const char *funcname;
    unsigned int dispatch_behaviour;
} nfs_function_desc_t;

/* 无效操作函数的相应处理类别 */
const nfs_function_desc_t invalid_funcdesc = {
        .service_function = nullptr,
        .free_function = nullptr,
        .xdr_decode_func = (xdrproc_t) xdr_void,
        .xdr_encode_func = (xdrproc_t) xdr_void,
        .funcname = "invalid_function",
        .dispatch_behaviour = NOTHING_SPECIAL,
};


typedef struct nfs_request {
    struct svc_req svc;
    struct dnfs_request_lookahead lookahead;
    nfs_arg_t arg_nfs;
    nfs_res_t *res_nfs;
    const nfs_function_desc_t *funcdesc;
    void *proc_data;
    /** This request may be queued up pending completion of the request
     *  this is a dupreq of.
     */
    TAILQ_ENTRY(nfs_request) dupes;
} nfs_request_t;


#endif //DNFSD_DNFS_META_DATA_H
