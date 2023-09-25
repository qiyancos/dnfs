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
#ifndef DNFSD_NFS_INIT_H
#define DNFSD_NFS_INIT_H

extern "C" {
#include "rpc/rpc.h"
}
#include "nfs_base.h"
#include "utils/net_utils.h"

/*服务udp句柄*/
extern SVCXPRT *udp_xprt;
/*服务tcp句柄*/
extern SVCXPRT *tcp_xprt;

/*udp协议套接字*/
extern int udp_socket;
/*tcp协议套接字*/
extern int tcp_socket;

/* NFSV3的socket绑定信息 */
extern proto_data nfsv3_sock_info;

/* 对于一个有效的NFS调用，找到匹配的处理函数调用函数处理请求并返回结果 */
enum xprt_stat nfs_rpc_valid_NFS(struct svc_req *req);

/* Dispatch after rendezvous，该函数用于在接收到指定协议的UDP数据，如NFSV23的RPC请求后
 * 执行该函数进行二次分发，分发给实际RPC对应的处理函数执行处理操作并返回数据 */
enum xprt_stat nfs_rpc_dispatch_udp_NFS(SVCXPRT *xprt);

/* 该函数用于在接收到指定协议的TCP数据 */
enum xprt_stat nfs_rpc_dispatch_tcp_NFS(SVCXPRT *xprt);

/*注册nfs服务
 * params netconfig_udpv4:主机udp4网络配置
 * params netconfig_tcpv4:主机tcp4网络配置
 * */
void nfs_init_svc(netconfig *netconfig_udpv4, netconfig *netconfig_tcpv4);

#endif //DNFSD_NFS_INIT_H
