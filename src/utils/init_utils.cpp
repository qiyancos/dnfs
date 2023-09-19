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
 *//*

#include <netinet/tcp.h>
#include "utils/init_utils.h"
#include "dnfsd/dnfs_config.h"

#define MODULE_NAME "DNFS"

*/
/* 为已经分配的套接字设置相关的属性信息
 * params udp_socket:udp协议套接字
 * *//*

static int socket_setopts(int udp_socket, int tcp_socket) {
    int one = 1;
    const struct nfs_core_param *nfs_cp = &nfs_param.core_param;

    */
/* Use SO_REUSEADDR in order to avoid wait the 2MSL timeout *//*

    if (setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR,
                   &one, sizeof(one))) {
        LOG(MODULE_NAME, L_ERROR,
            "Bad udp socket options reuseaddr for DNFS udp sock, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }

    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR,
                   &one, sizeof(one))) {
        LOG(MODULE_NAME, L_ERROR,
            "Bad tcp socket option reuseaddr for DNFS tcp sock, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }

    if (nfs_cp->enable_tcp_keepalive) {
        if (setsockopt(tcp_socket, SOL_SOCKET, SO_KEEPALIVE,
                       &one, sizeof(one))) {
            LOG(MODULE_NAME, L_ERROR,
                "Bad tcp socket option keepalive for %s, error %d(%s)",
                errno, strerror(errno));
            return -1;
        }

        if (nfs_cp->tcp_keepcnt) {
            if (setsockopt(tcp_socket, IPPROTO_TCP, TCP_KEEPCNT,
                           &nfs_cp->tcp_keepcnt,
                           sizeof(nfs_cp->tcp_keepcnt))) {
                LOG(MODULE_NAME, L_ERROR,
                    "Bad tcp socket option TCP_KEEPCNT for %s, error %d(%s)",
                    errno, strerror(errno));
                return -1;
            }
        }

        if (nfs_cp->tcp_keepidle) {
            if (setsockopt(tcp_socket, IPPROTO_TCP, TCP_KEEPIDLE,
                           &nfs_cp->tcp_keepidle,
                           sizeof(nfs_cp->tcp_keepidle))) {
                LOG(MODULE_NAME, L_ERROR,
                    "Bad tcp socket option TCP_KEEPIDLE for %s, error %d(%s)",
                    errno, strerror(errno));
                return -1;
            }
        }

        if (nfs_cp->tcp_keepintvl) {
            if (setsockopt(tcp_socket, IPPROTO_TCP,
                           TCP_KEEPINTVL, &nfs_cp->tcp_keepintvl,
                           sizeof(nfs_cp->tcp_keepintvl))) {
                LOG(MODULE_NAME, L_ERROR,
                    "Bad tcp socket option TCP_KEEPINTVL for %s, error %d(%s)",
                    errno, strerror(errno));
                return -1;
            }
        }
    }

    */
/* We prefer using non-blocking socket in the specific case *//*

    if (fcntl(udp_socket, F_SETFL, FNDELAY) == -1) {
        LOG(MODULE_NAME, L_ERROR,
            "Cannot set udp socket for %s as non blocking, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }

    return 0;
}

*/
/* 为DNFS分配socket套接字 *//*

static int allocate_sockets() {
    LOG(MODULE_NAME, D_INFO, "Allocation of the sockets");

    udp_socket = tcp_socket = -1;

    */
/* 首先为NFSV3分配UDP的监听接口 *//*

    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (udp_socket == -1) {
        if (errno == EAFNOSUPPORT) {
            LOG(MODULE_NAME, L_ERROR, "No V4 intfs configured?!");
        }

        LOG(MODULE_NAME, L_ERROR,
            "Cannot allocate a udp socket for DNFS, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }

    */
/* 首先为NFSV3分配TCP的监听接口 *//*

    tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (tcp_socket == -1) {
        LOG(MODULE_NAME, L_ERROR,
            "Cannot allocate a tcp socket for DNFS, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }
    return 0;
}

*/
/* NFSV3的socket绑定信息 *//*

proto_data nfsv3_sock_info;

*/
/* 将已经申请到的套接字绑定到指定的端口号上 *//*

static void bind_udp_sockets() {
    LOG(MODULE_NAME, D_INFO, "Binding to UDP address %s",
        format((sockaddr_storage *) &(nfs_param.core_param.bind_addr)).c_str());

    proto_data *pdatap = &nfsv3_sock_info;

    memset(&pdatap->sinaddr_udp, 0,
           sizeof(pdatap->sinaddr_udp));

    pdatap->sinaddr_udp.sin_family = AF_INET;

    */
/* all interfaces *//*

    pdatap->sinaddr_udp.sin_addr.s_addr =
            nfs_param.core_param.bind_addr.sin_addr.s_addr;
    pdatap->sinaddr_udp.sin_port =
            htons(nfs_param.core_param.port);

    pdatap->netbuf_udp6.maxlen =
            sizeof(pdatap->sinaddr_udp);
    pdatap->netbuf_udp6.len =
            sizeof(pdatap->sinaddr_udp);
    pdatap->netbuf_udp6.buf = &pdatap->sinaddr_udp;

    pdatap->bindaddr_udp6.qlen = SOMAXCONN;
    pdatap->bindaddr_udp6.addr =
            pdatap->netbuf_udp6;

    */
/* 从socket中获取ntirpc需要的信息并存储起来 *//*

    if (!__rpc_fd2sockinfo(udp_socket,
                           &pdatap->si_udp6)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Cannot get socket info for udp6 socket errno=%d (%s)",
            errno, strerror(errno));
    }

    LOG(MODULE_NAME, D_INFO, "Binding UDP socket to address %s",
        format((sockaddr_storage *) pdatap->bindaddr_udp6.addr.buf).c_str());

    int rc = bind(udp_socket,
                  (struct sockaddr *)
                          pdatap->bindaddr_udp6.addr.buf,
                  (socklen_t) pdatap->si_udp6.si_alen);
    if (rc == -1) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Cannot bind udp6 socket, error %d (%s)",
            errno, strerror(errno));
    }
}

*/
/* 将已经申请到的套接字绑定到指定的端口号上 *//*

static void bind_tcp_sockets() {
    LOG(MODULE_NAME, D_INFO, "Binding to TCP address %s",
        format((sockaddr_storage *) &(nfs_param.core_param.bind_addr)).c_str());

    proto_data *pdatap = &nfsv3_sock_info;

    memset(&pdatap->sinaddr_tcp, 0,
           sizeof(pdatap->sinaddr_tcp));
    pdatap->sinaddr_tcp.sin_family = AF_INET;
    */
/* all interfaces *//*

    pdatap->sinaddr_tcp.sin_addr.s_addr =
            nfs_param.core_param.bind_addr.sin_addr.s_addr;
    pdatap->sinaddr_tcp.sin_port =
            htons(nfs_param.core_param.port);

    pdatap->netbuf_tcp6.maxlen =
            sizeof(pdatap->sinaddr_tcp);
    pdatap->netbuf_tcp6.len = sizeof(pdatap->sinaddr_tcp);
    pdatap->netbuf_tcp6.buf = &pdatap->sinaddr_tcp;

    pdatap->bindaddr_tcp6.qlen = SOMAXCONN;
    pdatap->bindaddr_tcp6.addr = pdatap->netbuf_tcp6;

    if (!__rpc_fd2sockinfo(tcp_socket,
                           &pdatap->si_tcp6)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "V4 : Cannot get socket info for tcp socket error %d(%s)",
            errno, strerror(errno));
    }

    LOG(MODULE_NAME, D_INFO, "Binding TCP socket to address %s",
        format((sockaddr_storage *) pdatap->bindaddr_tcp6.addr.buf).c_str());

    int rc = bind(tcp_socket,
                  (struct sockaddr *)
                          pdatap->bindaddr_tcp6.addr.buf,
                  (socklen_t) pdatap->si_tcp6.si_alen);
    if (rc == -1) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Cannot bind tcp socket, error %d(%s)",
            errno, strerror(errno));
    }
}

struct netconfig *netconfig_udpv4;
struct netconfig *netconfig_tcpv4;

*/
/* 使用rpcbind取消当前rpc的绑定关系，以便绑定新的内容 *//*

static void unregister_rpc() {
    const rpcprog_t &prog = nfs_param.core_param.program;
    rpcb_unset(prog, NFS_V3, netconfig_udpv4);
    rpcb_unset(prog, NFS_V3, netconfig_tcpv4);
}

*/
/**
 * @brief xprt destructor callout
 *
 * @param[in] xprt Transport to destroy
 *//*

static enum xprt_stat nfs_rpc_free_user_data(SVCXPRT *xprt) {
    if (xprt->xp_u2) {
        */
/*TODO*//*

//        nfs_dupreq_put_drc(xprt->xp_u2);
//        xprt->xp_u2 = nullptr;
    }
    return XPRT_DESTROYED;
}

SVCXPRT *udp_xprt;
SVCXPRT *tcp_xprt;

*/
/* 给每一个协议创建相应的svcxprt网络传输句柄，每一个协议对应的每一个网络协议都
 * 有一个单独的XPRT传输句柄，用来执行后续的处理操作 *//*

void create_svcxprts() {
    LOG(MODULE_NAME, D_INFO, "Create new svc xprts");

    */
/* 创建UDP相关的XPRT *//*

    udp_xprt = svc_dg_create(udp_socket,
                             nfs_param.core_param.rpc.max_send_buffer_size,
                             nfs_param.core_param.rpc.max_recv_buffer_size);

    if (udp_xprt == nullptr) {
        LOG(MODULE_NAME, EXIT_ERROR, "Cannot allocate UDP SVCXPRT");
    }

    udp_xprt->xp_dispatch.rendezvous_cb = nfs_rpc_dispatch_udp_NFS;

    */
/* Hook xp_free_user_data (finalize/free private data) *//*

    (void) SVC_CONTROL(udp_xprt, SVCSET_XP_FREE_USER_DATA,
                       (void *) nfs_rpc_free_user_data);

    (void) svc_rqst_evchan_reg(rpc_evchan[UDP_UREG_CHAN].chan_id,
                               udp_xprt,
                               SVC_RQST_FLAG_XPRT_UREG);

    */
/* 创建TCP相关的XPRT *//*

    tcp_xprt = svc_vc_ncreatef(tcp_socket,
                               nfs_param.core_param.rpc.max_send_buffer_size,
                               nfs_param.core_param.rpc.max_recv_buffer_size,
                               SVC_CREATE_FLAG_CLOSE | SVC_CREATE_FLAG_LISTEN);
    if (tcp_xprt == nullptr) {
        LOG(MODULE_NAME, EXIT_ERROR, "Cannot allocate TCP SVCXPRT");
    }

    tcp_xprt->xp_dispatch.rendezvous_cb = nfs_rpc_dispatch_tcp_NFS;

    */
/* Hook xp_free_user_data (finalize/free private data) *//*

    (void) SVC_CONTROL(tcp_xprt, SVCSET_XP_FREE_USER_DATA,
                       (void *) nfs_rpc_free_user_data);

    (void) svc_rqst_evchan_reg(rpc_evchan[TCP_UREG_CHAN].chan_id,
                               tcp_xprt, SVC_RQST_FLAG_XPRT_UREG);
}

*/
/* 调用rpcbind服务绑定当前的函数到RPC调用操作上 *//*

static void register_rpc_program() {
    LOG(MODULE_NAME, D_INFO, "Registering V3/UDP");

    */
/* XXXX fix svc_register! *//*

    if (!svc_reg(udp_xprt, nfs_param.core_param.program, (u_long) NFS_V3,
                 nfs_rpc_dispatch_dummy, netconfig_udpv4)) {
        LOG(MODULE_NAME, EXIT_ERROR, "Cannot register V%d on UDP",
            (int) NFS_V3);
    }

    LOG(MODULE_NAME, D_INFO, "Registering V%d/TCP", (int) NFS_V3);

    if (!svc_reg(tcp_xprt, nfs_param.core_param.program, (u_long) NFS_V3,
                 nfs_rpc_dispatch_dummy, netconfig_tcpv4)) {
        LOG(MODULE_NAME, EXIT_ERROR, "Cannot register %s V%d on TCP",
            (int) NFS_V3);
    }
}

*/
/* 初始化nfs服务相关的接口注册操作 *//*

static void dnfs_init_svc() {
    */
/* Get the netconfig entries from /etc/netconfig *//*

    netconfig_udpv4 = (struct netconfig *) getnetconfigent("udp");
    if (netconfig_udpv4 == nullptr)
        LOG(MODULE_NAME, L_ERROR,
            "Cannot get udp netconfig, cannot get an entry for udp in netconfig file. Check file /etc/netconfig...");

    */
/* Get the netconfig entries from /etc/netconfig *//*

    netconfig_tcpv4 = (struct netconfig *) getnetconfigent("tcp");
    if (netconfig_tcpv4 == nullptr)
        LOG(MODULE_NAME, L_ERROR,
            "Cannot get tcp netconfig, cannot get an entry for tcp in netconfig file. Check file /etc/netconfig...");

    */
/* 初始化默认运行配置中的绑定地址数据 *//*

    if (!inet_pton(nfs_param.core_param.bind_addr.sin_family,
                   nfs_param.core_param.bind_addr_str.c_str(),
                   &nfs_param.core_param.bind_addr.sin_addr)) {
        LOG(MODULE_NAME, EXIT_ERROR, "Illegal bing ipv4 addr \"%s\" for nfsv3",
            nfs_param.core_param.bind_addr_str.c_str());
    }
    nfs_param.core_param.bind_addr.sin_port = htons(nfs_param.core_param.port);

    */
/* 为NFS_V3的协议分配UDP和TCP的套接字 *//*

    if (allocate_sockets()) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Failed to allocate socket for DNFS, error %d(%s)",
            errno, strerror(errno));
    }

    LOG(MODULE_NAME, D_INFO, "Socket numbers are: tcp=%d udp=%d",
        tcp_socket, udp_socket);

    */
/* 对分配的socket的属性进行设置 *//*

    if (socket_setopts()) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Error setting socket option for DNFS");
    }

    */
/* 将生成的套接字socket绑定到指定的端口号 *//*

    bind_udp_sockets();
    bind_tcp_sockets();
    LOG(MODULE_NAME, L_INFO, "Bind sockets successful");

    */
/* 从portmapper/rpcbind中取消当前已有rpc程序的绑定关系以便进行新的绑定 *//*

    unregister_rpc();

    */
/* 初始化网络传输的xprt句柄 *//*

    create_svcxprts();

    */
/* 对RPC的程序进行正式的注册，包括UDP和TCP，一旦完成注册，就可以开始接受rpc请求了 *//*

    register_rpc_program();
}
*/
