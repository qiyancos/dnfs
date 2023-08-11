//
// Created by iecas on 2023/8/11.
//

#ifndef DNFS_DNFS_RPC_WORKER_H
#define DNFS_DNFS_RPC_WORKER_H

#include <stdbool.h>

#include "rpc/svc.h"

typedef union nfs_arg__ {
    int test;
} nfs_arg_t;

typedef union nfs_res__ {
    int test;
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
    char *funcname;
    unsigned int dispatch_behaviour;
} nfs_function_desc_t;

typedef struct nfs_request {
    struct svc_req svc;
} nfs_request_t;

#define NOTHING_SPECIAL 0x0000	/* Nothing to be done for this kind of
				   request */
#define CAN_BE_DUP	0x0004	/* Handling of dup request can be done
				   for this request */

enum xprt_stat nfs_rpc_valid_NFS(struct svc_req *req);

#endif //DNFS_DNFS_RPC_WORKER_H
