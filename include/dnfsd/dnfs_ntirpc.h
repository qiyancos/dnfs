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

#ifndef DNFSD_DNFS_NTIRPC_H
#define DNFSD_DNFS_NTIRPC_H

#include "rpc/svc.h"

enum evchan {
    UDP_UREG_CHAN,		/*< Put UDP on a dedicated channel */
    TCP_UREG_CHAN,		/*< Accepts new TCP connections */
#ifdef _USE_NFS_RDMA
    RDMA_UREG_CHAN,		/*< Accepts new RDMA connections */
#endif
    EVCHAN_SIZE
};
#define N_TCP_EVENT_CHAN  3	/*< We don't really want to have too many,
				   relative to the number of available cores. */
#define N_EVENT_CHAN (N_TCP_EVENT_CHAN + EVCHAN_SIZE)

/* TIRPC的全局操作处理函数 */
extern tirpc_pkg_params ntirpc_pp;

/* 注册tirpc的处理操作参数 */
void init_ntirpc_settings();

// 用于ntirpc的警告信息输出和处理
void rpc_warnx(const char* format, ...);

// 该函数船体给tirpc，释放内存空间使用的函数
void rpc_free(void *p, size_t n);

// 该函数负责分配新的内存区域，并在分配失败的时候打印错误信息
void* rpc_malloc(size_t n, const char *file, int line, const char *function);

// 包含对齐的内存空间分配函数
void* rpc_malloc_aligned(size_t a, size_t n,
                         const char *file, int line, const char *function);

// 根据给定的结构体个数和结构体大小分配空间
void* rpc_calloc(size_t n, size_t s, const char *file, int line, const char *function);

// 根据新的大小对之前已经分配的内存区域进行重新分配，新的区域会复制之前区域的数据
void* rpc_realloc(void *p, size_t n, const char *file, int line, const char *function);

/* 为一个新的dnfs请求申请空间并进行相关的初始化操作 */
struct svc_req *alloc_dnfs_request(SVCXPRT *xprt, XDR *xdrs);

/* 释放请求数据结构体对应的内存空间并执行其他销毁操作 */
void free_dnfs_request(struct svc_req *req, enum xprt_stat stat);

/* Dispatch after rendezvous，该函数用于在接收到指定协议的UDP数据，如NFSV23的RPC请求后
 * 执行该函数进行二次分发，分发给实际RPC对应的处理函数执行处理操作并返回数据 */
enum xprt_stat nfs_rpc_dispatch_udp_NFS(SVCXPRT *xprt);

/* 该函数用于在接收到指定协议的TCP数据 */
enum xprt_stat nfs_rpc_dispatch_tcp_NFS(SVCXPRT *xprt);

/* 一个占位函数，基本不会使用到 */
void nfs_rpc_dispatch_dummy([[maybe_unused]] struct svc_req *req);

#endif //DNFSD_DNFS_NTIRPC_H
