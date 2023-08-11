//
// Created by iecas on 2023/8/10.
//

#ifndef DNFS_DNFS_RPC_DISPATCHER_H
#define DNFS_DNFS_RPC_DISPATCHER_H

#include <stdint.h>
#include <netinet/tcp.h>
#include "nfs/nfs23.h"
#include "nfs/nfsv41.h"
#include "rpc/svc.h"
#include "rpc/svc_rqst.h"
#include "rpc/rpcb_clnt.h"

#include "dnfsd/dnfs_init.h"
#include "log/display.h"

enum evchan {
    UDP_UREG_CHAN,		/*< Put UDP on a dedicated channel */
    TCP_UREG_CHAN,		/*< Accepts new TCP connections */
#ifdef _USE_NFS_RDMA
    RDMA_UREG_CHAN,		/*< Accepts new RDMA connections */
#endif
    EVCHAN_SIZE
};

struct rpc_evchan {
    uint32_t chan_id;	/*< Channel ID */
};

#define N_TCP_EVENT_CHAN  3	/*< We don't really want to have too many,
				   relative to the number of available cores. */

#define N_EVENT_CHAN (N_TCP_EVENT_CHAN + EVCHAN_SIZE)

#define UDP_LISTENER_NONE	0
#define UDP_LISTENER_ALL	0x00000001
#define UDP_LISTENER_MOUNT	0x00000002
#define UDP_LISTENER_MASK (UDP_LISTENER_ALL | UDP_LISTENER_MOUNT)

#define NFS_options nfs_param.core_param.core_options
#define NFS_program nfs_param.core_param.program

#define CORE_OPTION_NFSV3 0x00000001
#define CORE_OPTION_NFSV4 0x00000002
#define CORE_OPTION_ALL_NFS_VERS (CORE_OPTION_NFSV3 | CORE_OPTION_NFSV4)

/* Allow much more space than we really need for a sock name. An IPV4 address
 * embedded in IPv6 could use 45 bytes and then if we add a port, that would be
 * an additional 6 bytes (:65535) for a total of 51, and then one more for NUL
 * termination. We could use 64 instead of 128.
 */
#define SOCK_NAME_MAX 128

typedef struct proto_data {
    struct sockaddr_in sinaddr_udp;
    struct sockaddr_in sinaddr_tcp;
    struct sockaddr_in6 sinaddr_udp6;
    struct sockaddr_in6 sinaddr_tcp6;
    struct netbuf netbuf_udp6;
    struct netbuf netbuf_tcp6;
    struct t_bind bindaddr_udp6;
    struct t_bind bindaddr_tcp6;
    struct __rpc_sockinfo si_udp6;
    struct __rpc_sockinfo si_tcp6;
} proto_data;

enum xprt_stat nfs_rpc_dispatch_tcp_NFS(SVCXPRT *xprt);

void nfs_Init_svc(void);
void Clean_RPC(void);

#endif //DNFS_DNFS_RPC_DISPATCHER_H
