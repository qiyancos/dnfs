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
#ifndef DNFSD_NET_UTILS_H
#define DNFSD_NET_UTILS_H

#include <netinet/in.h>
#include <string>
#include "rpc/types.h"

/*每个服务的socket绑定信息*/
typedef struct proto_data {
    struct sockaddr_in sinaddr_udp;
    struct sockaddr_in sinaddr_tcp;
//    struct sockaddr_in6 sinaddr_udp6;
//    struct sockaddr_in6 sinaddr_tcp6;
    struct netbuf netbuf_udp6;
    struct netbuf netbuf_tcp6;
    struct t_bind bindaddr_udp6;
    struct t_bind bindaddr_tcp6;
    struct __rpc_sockinfo si_udp6;
    struct __rpc_sockinfo si_tcp6;
} proto_data;

enum evchan {
    UDP_UREG_CHAN,        /*< Put UDP on a dedicated channel */
    TCP_UREG_CHAN,        /*< Accepts new TCP connections */
#ifdef _USE_NFS_RDMA
    RDMA_UREG_CHAN,		/*< Accepts new RDMA connections */
#endif
    EVCHAN_SIZE
};

/**
 * TI-RPC event channels.  Each channel is a thread servicing an event
 * demultiplexer.
 */
struct rpc_evchan {
    uint32_t chan_id;    /*< Channel ID */
};
static struct rpc_evchan rpc_evchan[EVCHAN_SIZE];

/* 分配socket套接字
 * params svc_name:服务名称
 * params udp_socket:udp协议套接字
 * params tcp_socket:tcp协议套接字
 * */
static int
allocate_sockets(const std::string &svc_name, int &udp_socket, int &tcp_socket);

/* 为已经分配的套接字设置相关的属性信息
 * params svc_name:服务名称
 * params udp_socket:udp协议套接字
 * params tcp_socket:tcp协议套接字
 * */
static int socket_setopts(const std::string &svc_name, int &udp_socket, int &tcp_socket);

/* 将socket绑定到制定端口
 * params svc_name:服务名称
 * params sock_info:单个服务的socket绑定信息
 * params udp_socket:服务udp socket
 * params port:绑定的端口
 * */
static void bind_udp_sockets(const std::string &svc_name, proto_data &sock_info,
                             const int &udp_socket, const uint16_t &port);

/* 将socket绑定到制定端口
 * params svc_name:服务名称
 * params sock_info:单个服务的socket绑定信息
 * params tcp_socket:服务tcp socket
 * params port:绑定的端口
 * */
static void bind_tcp_sockets(const std::string &svc_name, proto_data &sock_info,
                             const int &tcp_socket, const uint16_t &port);

/* 取消项目的rpc绑定
 * params netconfig_udpv4:主机udp4网络配置
 * params netconfig_tcpv4:主机tcp4网络配置
 * params program_ver:服务版本
 * params program_number:项目注册编号
 * */
static void unregister_rpc(netconfig *netconfig_udpv4, netconfig *netconfig_tcpv4,
                           const u_long &program_ver,
                           const rpcprog_t &program_number);

static enum xprt_stat nfs_rpc_free_user_data(SVCXPRT *xprt);

/* 给每一个协议创建相应的udp svcxprt网络传输句柄，每一个协议对应的每一个网络协议都
 * 有一个单独的XPRT传输句柄，用来执行后续的处理操作
 * params udp_xprt:创建的udp句柄
 * params svc_name:服务名称
 * params udp_socket:udp协议套接字
  * params udp_xprt_func:udp接口处理方法
 * */
void create_udp_svcxprts(SVCXPRT **udp_xprt, const std::string &svc_name,
                         int &udp_socket, xprt_stat (*udp_xprt_func)(SVCXPRT *xprt));

/* 给每一个协议创建相应的tcp svcxprt网络传输句柄，每一个协议对应的每一个网络协议都
 * 有一个单独的XPRT传输句柄，用来执行后续的处理操作
 * params tcp_xprt:创建的tcp句柄
 * params svc_name:服务名称
 * params tcp_socket:tcp协议套接字
 * params tcp_xprt_func:tcp接口处理方法
 * */
void create_tcp_svcxprts(SVCXPRT **tcp_xprt, const std::string &svc_name,
                         int &tcp_socket, xprt_stat (*tcp_xprt_func)(SVCXPRT *xprt));

/* 一个占位函数，基本不会使用到 */
void nfs_rpc_dispatch_dummy([[maybe_unused]] struct svc_req *req);

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
                     const std::string &svc_name, const rpcprog_t &program_number);

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
              const rpcprog_t &program_number);

#endif //DNFSD_NET_UTILS_H
