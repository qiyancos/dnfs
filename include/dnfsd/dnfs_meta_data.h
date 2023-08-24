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

#include "nfs/nfsv41.h"
#include "nfs/nfs23.h"

struct dnfs_request_lookahead {
    uint32_t flags;
    uint16_t read;
    uint16_t write;
};

typedef struct nfs_core_param {
    /** An array of port numbers, one for each protocol.  Set by
        the NFS_Port, MNT_Port, NLM_Port, and Rquota_Port options. */
    uint16_t port;
    /** The IPv4 or IPv6 address to which to bind for our
        listening port.  Set by the Bind_Addr option. */
    sockaddr_storage bind_addr;
    /** An array of RPC program numbers.  The correct values, by
        default, they may be set to incorrect values with the
        NFS_Program, MNT_Program, NLM_Program, and
        Rquota_Program.  It is debatable whether this is a
        worthwhile option to have. */
    uint32_t program;
    /** Parameters affecting the relation with TIRPC.   */
    struct {
        /** Maximum number of connections for TIRPC.
            Defaults to 1024 and settable by
            RPC_Max_Connections. */
        uint32_t max_connections;
        /** Size of RPC send buffer.  Defaults to
            NFS_DEFAULT_SEND_BUFFER_SIZE and is settable by
            MaxRPCSendBufferSize.  */
        uint32_t max_send_buffer_size;
        /** Size of RPC receive buffer.  Defaults to
            NFS_DEFAULT_RECV_BUFFER_SIZE and is settable by
            MaxRPCRecvBufferSize. */
        uint32_t max_recv_buffer_size;
        /** Idle timeout (seconds).  Defaults to 5m */
        uint32_t idle_timeout_s;
        /** TIRPC ioq min simultaneous io threads.  Defaults to
            2 and settable by rpc_ioq_thrdmin. */
        uint32_t ioq_thrd_min;
        /** TIRPC ioq max simultaneous io threads.  Defaults to
            200 and settable by RPC_Ioq_ThrdMax. */
        uint32_t ioq_thrd_max;
        struct {
            /** Partitions in GSS ctx cache table (default 13). */
            uint32_t ctx_hash_partitions;
            /** Max GSS contexts in cache (i.e.,
             * max GSS clients, default 16K)
             */
            uint32_t max_ctx;
            /** Max entries to expire in one idle
             * check (default 200)
             */
            uint32_t max_gc;
        } gss;
    } rpc;
    /** Protocols to support.  Should probably be renamed.
        Defaults to CORE_OPTION_ALL_VERS and is settable with
        NFS_Protocols (as a comma-separated list of 3 and 4.) */
    unsigned int core_options;
    /** Whether tcp sockets should use SO_KEEPALIVE */
    bool enable_tcp_keepalive;
    /** Maximum number of TCP probes before dropping the connection */
    uint32_t tcp_keepcnt;
    /** Idle time before TCP starts to send keepalive probes */
    uint32_t tcp_keepidle;
    /** Time between each keepalive probe */
    uint32_t tcp_keepintvl;
} nfs_core_parameter_t;

typedef struct nfs_param {
    /* NFS的核心参数数据，主要是针对NFSV23协议的内容 */
    nfs_core_parameter_t core_param;
} nfs_parameter_t;

typedef enum protos {
    P_NFS,			/*< NFS, of course. */
    P_MNT,			/*< Mount (for v3) */
    P_COUNT			/*< Number of protocols */
} protos;

typedef union nfs_arg__ {
} nfs_arg_t;

typedef union nfs_res__ {
    struct COMPOUND4res_extended *res_compound4_extended;
    /* mount */
    fhstatus2 res_mnt1;
} nfs_res_t;

typedef int (*nfs_protocol_function_t) (nfs_arg_t *,
                                        struct svc_req *,
                                        nfs_res_t *);

typedef void (*nfs_protocol_free_t) (nfs_res_t *);

typedef struct nfs_function_desc__ {
    nfs_protocol_function_t service_function;
    nfs_protocol_free_t free_function;
    xdrproc_t xdr_decode_func;
    xdrproc_t xdr_encode_func;
    const char *funcname;
    unsigned int dispatch_behaviour;
} nfs_function_desc_t;

typedef struct nfs_request {
    struct svc_req svc;
    struct dnfs_request_lookahead lookahead;
//    nfs_arg_t arg_nfs;
//    nfs_res_t *res_nfs;
    const nfs_function_desc_t *funcdesc;
    void *proc_data;
    /** This request may be queued up pending completion of the request
     *  this is a dupreq of.
     */
    TAILQ_ENTRY(nfs_request) dupes;
} nfs_request_t;

#define NOTHING_SPECIAL 0x0000	/* Nothing to be done for this kind of
				   request */

#endif //DNFSD_DNFS_META_DATA_H
