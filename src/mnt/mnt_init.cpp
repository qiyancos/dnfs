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
#include "mnt/mnt_init.h"
#include "mnt/mnt_base.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "utils/common_utils.h"
#include "dnfsd/dnfs_ntirpc.h"

#define MODULE_NAME "MNT"

/*服务udp句柄*/
SVCXPRT *mnt_udp_xprt;
/*服务tcp句柄*/
SVCXPRT *mnt_tcp_xprt;

/*udp协议套接字*/
int mnt_udp_socket;
/*tcp协议套接字*/
int mnt_tcp_socket;

/* NFSV3的socket绑定信息 */
proto_data mntv3_sock_info;

/* 对于一个有效的NFS调用，找到匹配的处理函数调用函数处理请求并返回结果 */
enum xprt_stat nfs_rpc_valid_MNT(struct svc_req *req) {

    nfs_request_t *reqdata = get_parent_struct_addr(
            req, struct nfs_request, svc);

    reqdata->funcdesc = &invalid_funcdesc;

    if (req->rq_msg.cb_prog != MNT_PROGRAM) {
        return nfs_rpc_noprog(reqdata);
    }

    if (req->rq_msg.cb_vers == MOUNT_V3) {
        if (req->rq_msg.cb_proc <= MOUNTPROC3_EXPORT) {
            reqdata->funcdesc =
                    &mnt3_func_desc[req->rq_msg.cb_proc];
            return nfs_rpc_process_request(reqdata, false);
        }
        return nfs_rpc_noproc(reqdata);
    }

    return nfs_rpc_novers(reqdata);
}

/* Dispatch after rendezvous，该函数用于在接收到指定协议的UDP数据，如NFSV23的RPC请求后
 * 执行该函数进行二次分发，分发给实际RPC对应的处理函数执行处理操作并返回数据 */
enum xprt_stat nfs_rpc_dispatch_udp_MNT(SVCXPRT *xprt) {
    LOG(MODULE_NAME, D_INFO,
        "'%s' UDP request for SVCXPRT %p fd %d", MODULE_NAME,
        xprt, xprt->xp_fd);
    xprt->xp_dispatch.process_cb = nfs_rpc_valid_MNT;
    return SVC_RECV(xprt);
}

/* 该函数用于在接收到指定协议的TCP数据 */
enum xprt_stat nfs_rpc_dispatch_tcp_MNT(SVCXPRT *xprt) {
    LOG(MODULE_NAME, D_INFO,
        "'%s' TCP request on SVCXPRT %p fd %d", MODULE_NAME,
        xprt, xprt->xp_fd);
    xprt->xp_dispatch.process_cb = nfs_rpc_valid_MNT;
    return SVC_STAT(xprt->xp_parent);
}

/*注册nfs服务
 * params netconfig_udpv4:主机udp4网络配置
 * params netconfig_tcpv4:主机tcp4网络配置
 * */
void mnt_init_svc(netconfig *netconfig_udpv4, netconfig *netconfig_tcpv4) {
    init_svc(mnt_udp_socket, mnt_tcp_socket,
             &mnt_udp_xprt, &mnt_tcp_xprt,
             netconfig_udpv4, netconfig_tcpv4,
             nfs_rpc_dispatch_udp_MNT,
             nfs_rpc_dispatch_tcp_MNT,
             MOUNT_V3,
             MODULE_NAME,
             mntv3_sock_info,
             MNT_PORT,
             MNT_PROGRAM);
}