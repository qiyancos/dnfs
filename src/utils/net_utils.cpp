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
extern "C" {
#include "rpc/rpc.h"
#include "rpc/rpcb_clnt.h"
#include <rpc/svc_rqst.h>
}

#include <netinet/tcp.h>
#include "utils/net_utils.h"
#include "dnfsd/dnfs_config.h"

using namespace std;

#define MODULE_NAME "net"

/* 分配socket套接字
 * params svc_name:服务名称
 * params udp_socket:udp协议套接字
 * params tcp_socket:tcp协议套接字
 * */

static int allocate_sockets(const string &svc_name, int &udp_socket, int &tcp_socket) {
    LOG(MODULE_NAME, D_INFO, "%s Allocation of the sockets",
        svc_name.c_str());

    udp_socket = tcp_socket = -1;

    /* 首先为NFSV3分配UDP的监听接口 */
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (udp_socket == -1) {
        if (errno == EAFNOSUPPORT) {
            LOG(MODULE_NAME, L_ERROR, "No V4 intfs configured?!");
        }

        LOG(MODULE_NAME, L_ERROR,
            "Cannot allocate a udp socket for %s, error %d(%s)",
            svc_name.c_str(),
            errno, strerror(errno));
        return -1;
    }

    /* 首先为NFSV3分配TCP的监听接口 */
    tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (tcp_socket == -1) {
        LOG(MODULE_NAME, L_ERROR,
            "Cannot allocate a tcp socket for %s, error %d(%s)",
            svc_name.c_str(),
            errno, strerror(errno));
        return -1;
    }
    return 0;
}

/* 为已经分配的套接字设置相关的属性信息
 * params svc_name:服务名称
 * params udp_socket:udp协议套接字
 * params tcp_socket:tcp协议套接字
 * */
static int socket_setopts(const string &svc_name, int &udp_socket, int &tcp_socket) {
    int one = 1;
    const struct nfs_core_param *nfs_cp = &nfs_param.core_param;


    /* Use SO_REUSEADDR in order to avoid wait the 2MSL timeout */
    if (setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR,
                   &one, sizeof(one))) {
        LOG(MODULE_NAME, L_ERROR,
            "Bad udp socket options reuseaddr for %s udp sock, error %d(%s)",
            svc_name.c_str(),
            errno, strerror(errno));
        return -1;
    }

    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR,
                   &one, sizeof(one))) {
        LOG(MODULE_NAME, L_ERROR,
            "Bad tcp socket option reuseaddr for %s tcp sock, error %d(%s)",
            svc_name.c_str(),
            errno, strerror(errno));
        return -1;
    }

    if (nfs_cp->enable_tcp_keepalive) {
        if (setsockopt(tcp_socket, SOL_SOCKET, SO_KEEPALIVE,
                       &one, sizeof(one))) {
            LOG(MODULE_NAME, L_ERROR,
                "Bad tcp socket option keepalive for %s, error %d(%s)",
                svc_name.c_str(),
                errno, strerror(errno));
            return -1;
        }

        if (nfs_cp->tcp_keepcnt) {
            if (setsockopt(tcp_socket, IPPROTO_TCP, TCP_KEEPCNT,
                           &nfs_cp->tcp_keepcnt,
                           sizeof(nfs_cp->tcp_keepcnt))) {
                LOG(MODULE_NAME, L_ERROR,
                    "Bad tcp socket option TCP_KEEPCNT for %s, error %d(%s)",
                    svc_name.c_str(),
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
                    svc_name.c_str(),
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
                    svc_name.c_str(),
                    errno, strerror(errno));
                return -1;
            }
        }
    }

    /* We prefer using non-blocking socket in the specific case */


    if (fcntl(udp_socket, F_SETFL, FNDELAY) == -1) {
        LOG(MODULE_NAME, L_ERROR,
            "Cannot set udp socket for %s as non blocking, error %d(%s)",
            svc_name.c_str(),
            errno, strerror(errno));
        return -1;
    }

    return 0;
}


/* 将socket绑定到制定端口
 * params svc_name:服务名称
 * params sock_info:单个服务的socket绑定信息
 * params udp_socket:udp协议套接字
 * params port:绑定的端口
 * */

static void bind_udp_sockets(const string &svc_name, proto_data &sock_info,
                             const int &udp_socket, const uint16_t &port) {
    LOG(MODULE_NAME, D_INFO, "Binding to %s UDP address %s",
        svc_name.c_str(),
        format((sockaddr_storage *) &(nfs_param.core_param.bind_addr)).c_str());

    proto_data *pdatap = &sock_info;

    memset(&pdatap->sinaddr_udp, 0,
           sizeof(pdatap->sinaddr_udp));

    pdatap->sinaddr_udp.sin_family = AF_INET;


    /* all interfaces */
    pdatap->sinaddr_udp.sin_addr.s_addr =
            nfs_param.core_param.bind_addr.sin_addr.s_addr;
    pdatap->sinaddr_udp.sin_port =
            htons(port);

    pdatap->netbuf_udp6.maxlen =
            sizeof(pdatap->sinaddr_udp);
    pdatap->netbuf_udp6.len =
            sizeof(pdatap->sinaddr_udp);
    pdatap->netbuf_udp6.buf = &pdatap->sinaddr_udp;

    pdatap->bindaddr_udp6.qlen = SOMAXCONN;
    pdatap->bindaddr_udp6.addr =
            pdatap->netbuf_udp6;


    /* 从socket中获取ntirpc需要的信息并存储起来 */
    if (!__rpc_fd2sockinfo(udp_socket,
                           &pdatap->si_udp6)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Cannot get socket info for udp6 socket set for %s errno=%d (%s)",
            svc_name.c_str(), errno, strerror(errno));
    }

    LOG(MODULE_NAME, D_INFO,
        "Binding UDP socket to address %s for %s",
        svc_name.c_str(),
        format((sockaddr_storage *) pdatap->bindaddr_udp6.addr.buf).c_str());

    int rc = bind(udp_socket,
                  (struct sockaddr *)
                          pdatap->bindaddr_udp6.addr.buf,
                  (socklen_t) pdatap->si_udp6.si_alen);
    if (rc == -1) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Cannot bind udp6 socket to %s, error %d (%s)",
            svc_name.c_str(), errno, strerror(errno));
    }
}

/* 将socket绑定到制定端口
 * params svc_name:服务名称
 * params sock_info:单个服务的socket绑定信息
 * params tcp_socket:tcp协议套接字
 * params port:绑定的端口
 * */

static void bind_tcp_sockets(const string &svc_name, proto_data &sock_info,
                             const int &tcp_socket, const uint16_t &port) {
    LOG(MODULE_NAME, D_INFO, "Binding to %s TCP address %s",
        svc_name.c_str(),
        format((sockaddr_storage *) &(nfs_param.core_param.bind_addr)).c_str());

    proto_data *pdatap = &sock_info;

    memset(&pdatap->sinaddr_tcp, 0,
           sizeof(pdatap->sinaddr_tcp));
    pdatap->sinaddr_tcp.sin_family = AF_INET;

    /* all interfaces */
    pdatap->sinaddr_tcp.sin_addr.s_addr =
            nfs_param.core_param.bind_addr.sin_addr.s_addr;
    pdatap->sinaddr_tcp.sin_port =
            htons(port);

    pdatap->netbuf_tcp6.maxlen =
            sizeof(pdatap->sinaddr_tcp);
    pdatap->netbuf_tcp6.len = sizeof(pdatap->sinaddr_tcp);
    pdatap->netbuf_tcp6.buf = &pdatap->sinaddr_tcp;

    pdatap->bindaddr_tcp6.qlen = SOMAXCONN;
    pdatap->bindaddr_tcp6.addr = pdatap->netbuf_tcp6;

    if (!__rpc_fd2sockinfo(tcp_socket,
                           &pdatap->si_tcp6)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "V4 : Cannot get socket info for tcp socket set for %s error %d(%s)",
            svc_name.c_str(),
            errno, strerror(errno));
    }

    LOG(MODULE_NAME, D_INFO, "Binding TCP socket to address %s for %s",
        svc_name.c_str(),
        format((sockaddr_storage *) pdatap->bindaddr_tcp6.addr.buf).c_str());

    int rc = bind(tcp_socket,
                  (struct sockaddr *)
                          pdatap->bindaddr_tcp6.addr.buf,
                  (socklen_t) pdatap->si_tcp6.si_alen);
    if (rc == -1) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Cannot bind tcp socket to %s, error %d(%s)",
            svc_name.c_str(),
            errno, strerror(errno));
    }
}

/* 取消项目的rpc绑定
 * params netconfig_udpv4:主机udp4网络配置
 * params netconfig_tcpv4:主机tcp4网络配置
 * params program_ver:服务版本
 * params program_number:项目注册编号
 * */
static void unregister_rpc(netconfig *netconfig_udpv4, netconfig *netconfig_tcpv4,const u_long &program_ver,
                           const rpcprog_t &program_number) {
    rpcb_unset(program_number, program_ver, netconfig_udpv4);
    rpcb_unset(program_number, program_ver, netconfig_tcpv4);
}

/**
   销毁传输
 */
static enum xprt_stat nfs_rpc_free_user_data(SVCXPRT *xprt) {
    if (xprt->xp_u2) {

//        nfs_dupreq_put_drc(xprt->xp_u2);
//        xprt->xp_u2 = nullptr;
    }
    return XPRT_DESTROYED;
}

/* 给每一个协议创建相应的udp svcxprt网络传输句柄，每一个协议对应的每一个网络协议都
 * 有一个单独的XPRT传输句柄，用来执行后续的处理操作
 * params udp_xprt:创建的udp句柄
 * params svc_name:服务名称
 * params udp_socket:udp协议套接字
  * params udp_xprt_func:udp接口处理方法
 * */
void create_udp_svcxprts(SVCXPRT **udp_xprt, const std::string &svc_name,
                         int &udp_socket, xprt_stat (*udp_xprt_func)(SVCXPRT *xprt)) {
    LOG(MODULE_NAME, D_INFO, "Create new %s xprts", svc_name.c_str());

    /* 创建UDP相关的XPRT */
    *udp_xprt = svc_dg_create(udp_socket,
                              nfs_param.core_param.rpc.max_send_buffer_size,
                              nfs_param.core_param.rpc.max_recv_buffer_size);

    if (*udp_xprt == nullptr) {
        LOG(MODULE_NAME, EXIT_ERROR, "%s Cannot allocate UDP SVCXPRT",
            svc_name.c_str());
    }

    (*udp_xprt)->xp_dispatch.rendezvous_cb = (svc_xprt_fun_t) udp_xprt_func;

    /* Hook xp_free_user_data (finalize/free private data) */
    (void) SVC_CONTROL(*udp_xprt, SVCSET_XP_FREE_USER_DATA,
                       (void *) nfs_rpc_free_user_data);

    (void) svc_rqst_evchan_reg(rpc_evchan[UDP_UREG_CHAN].chan_id,
                               *udp_xprt,
                               SVC_RQST_FLAG_XPRT_UREG);
}

/* 给每一个协议创建相应的tcp svcxprt网络传输句柄，每一个协议对应的每一个网络协议都
 * 有一个单独的XPRT传输句柄，用来执行后续的处理操作
 * params tcp_xprt:创建的tcp句柄
 * params svc_name:服务名称
 * params tcp_socket:tcp协议套接字
 * params tcp_xprt_func:tcp接口处理方法
 * */
void create_tcp_svcxprts(SVCXPRT **tcp_xprt, const std::string &svc_name,
                         int &tcp_socket, xprt_stat (*tcp_xprt_func)(SVCXPRT *xprt)) {
    LOG(MODULE_NAME, D_INFO, "Create new %s xprts", svc_name.c_str());

    /* 创建TCP相关的XPRT */
    *tcp_xprt = svc_vc_ncreatef(tcp_socket,
                                nfs_param.core_param.rpc.max_send_buffer_size,
                                nfs_param.core_param.rpc.max_recv_buffer_size,
                                SVC_CREATE_FLAG_CLOSE | SVC_CREATE_FLAG_LISTEN);
    if (*tcp_xprt == nullptr) {
        LOG(MODULE_NAME, EXIT_ERROR, "%s Cannot allocate TCP SVCXPRT",
            svc_name.c_str());
    }

    (*tcp_xprt)->xp_dispatch.rendezvous_cb = (svc_xprt_fun_t) tcp_xprt_func;

    /* Hook xp_free_user_data (finalize/free private data) */
    (void) SVC_CONTROL(*tcp_xprt, SVCSET_XP_FREE_USER_DATA,
                       (void *) nfs_rpc_free_user_data);

    (void) svc_rqst_evchan_reg(rpc_evchan[TCP_UREG_CHAN].chan_id,
                               *tcp_xprt, SVC_RQST_FLAG_XPRT_UREG);
}


/* 一个占位函数，基本不会使用到 */
void nfs_rpc_dispatch_dummy([[maybe_unused]] struct svc_req *req) {
    LOG(MODULE_NAME, L_ERROR,
        "Possible error, function %s should never be called",
        __func__);
}

/* 调用rpcbind服务绑定当前的函数到RPC调用操作上
 * params udp_xprt:创建的udp句柄
 * params tcp_xprt:创建的tcp句柄
 * params netconfig_udpv4:主机udp4网络配置
 * params netconfig_tcpv4:主机tcp4网络配置
 * params program_ver:注册项目版本
 * params svc_name:服务名称
 * params program_number:项目注册编号
 * */
static void
register_rpc_program(SVCXPRT **udp_xprt, SVCXPRT **tcp_xprt, netconfig *netconfig_udpv4,
                     netconfig *netconfig_tcpv4, u_long program_ver,
                     const string &svc_name, const rpcprog_t &program_number) {
    LOG(MODULE_NAME, D_INFO, "%s Registering V%d/UDP", svc_name.c_str(), program_ver);

    /* XXXX fix svc_register! */
    if (!svc_reg(*udp_xprt, program_number, program_ver,
                 nfs_rpc_dispatch_dummy, netconfig_udpv4)) {
        LOG(MODULE_NAME, EXIT_ERROR, "%s Cannot register V%d on UDP", svc_name.c_str(),
            program_ver);
    }

    LOG(MODULE_NAME, D_INFO, "%s Registering V%d/TCP", svc_name.c_str(), program_ver);

    if (!svc_reg(*tcp_xprt, program_number, program_ver,
                 nfs_rpc_dispatch_dummy, netconfig_tcpv4)) {
        LOG(MODULE_NAME, EXIT_ERROR, "%s Cannot register V%d on TCP", svc_name.c_str(),
            program_ver);
    }
}

/* 初始化nfs服务相关的接口注册操作
 * params udp_socket:udp协议套接字
 * params tcp_socket:tcp协议套接字
 * params udp_xprt:创建的udp句柄
 * params tcp_xprt:创建的tcp句柄
 * params netconfig_udpv4:主机udp4网络配置
 * params netconfig_tcpv4:主机tcp4网络配置
 * params udp_xprt_func:udp接口处理方法
 * params tcp_xprt_func:tcp接口处理方法
 * params program_ver:注册项目版本
 * params svc_name:服务名称
 * params sock_info:单个服务的socket绑定信息
 * params port:绑定的端口
 * params program_number:项目注册编号
 * */
void init_svc(int &udp_socket, int &tcp_socket,
              SVCXPRT **udp_xprt, SVCXPRT **tcp_xprt,
              netconfig *netconfig_udpv4, netconfig *netconfig_tcpv4,
              xprt_stat (*udp_xprt_func)(SVCXPRT *xprt),
              xprt_stat (*tcp_xprt_func)(SVCXPRT *xprt),
              u_long program_ver,
              const std::string &svc_name,
              proto_data &sock_info,
              const uint16_t &port,
              const rpcprog_t &program_number) {

    /* 为项目分配UDP和TCP的套接字 */
    if (allocate_sockets(svc_name, udp_socket, tcp_socket)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Failed to allocate socket for %s, error %d(%s)", svc_name.c_str(),
            errno, strerror(errno));
    }
    LOG(MODULE_NAME, D_INFO, "%s Socket numbers are: tcp=%d udp=%d", svc_name.c_str(),
        tcp_socket, udp_socket);

    /* 对分配的socket的属性进行设置 */
    if (socket_setopts(svc_name, udp_socket, tcp_socket)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Error setting socket option for %s", svc_name.c_str());
    }

    /* 将生成的套接字socket绑定到指定的端口号 */
    bind_udp_sockets(svc_name, sock_info, udp_socket, port);
    bind_tcp_sockets(svc_name, sock_info, tcp_socket, port);
    LOG(MODULE_NAME, L_INFO, "%s Bind sockets successful", svc_name.c_str());

    /* 从portmapper/rpcbind中取消当前已有rpc程序的绑定关系以便进行新的绑定 */
    unregister_rpc(netconfig_udpv4, netconfig_tcpv4,program_ver, program_number);

    /* 初始化网络传输的xprt句柄 */
    create_udp_svcxprts(udp_xprt, svc_name, udp_socket, udp_xprt_func);
    create_tcp_svcxprts(tcp_xprt, svc_name, tcp_socket, tcp_xprt_func);


    /* 对RPC的程序进行正式的注册，包括UDP和TCP，一旦完成注册，就可以开始接受rpc请求了 */
    register_rpc_program(udp_xprt, tcp_xprt, netconfig_udpv4, netconfig_tcpv4,
                         program_ver, svc_name, program_number);
}

