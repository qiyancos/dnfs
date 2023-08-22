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

#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "rpc/rpc.h"
#include "rpc/svc.h"
#include "log/log.h"
#include "utils/common_utils.h"
#include "utils/thread_utils.h"
#include "dnfsd/dnfs_meta_data.h"
#include "dnfsd/dnfs_ntirpc.h"

using namespace std;

#define MODULE_NAME "rpc"

// tirpc的控制参数集合
tirpc_pkg_params ntirpc_pp = {
        TIRPC_DEBUG_FLAG_DEFAULT,
        0,
        ThreadPool::set_thread_name,
        rpc_warnx,
        rpc_free,
        rpc_malloc,
        rpc_malloc_aligned,
        rpc_calloc,
        rpc_realloc, // 根据新的大小对之前已经分配的内存区域进行重新分配，新的区域会复制之前区域的数据
};

/* 注册tirpc的处理操作参数 */
void init_ntirpc_settings() {
    if (!tirpc_control(TIRPC_PUT_PARAMETERS, &ntirpc_pp)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Setting nTI-RPC parameters failed");
    }
}

// 用于ntirpc的警告信息输出和处理
void rpc_warnx(const char* format, ...) {
    va_list args;
    LOG(MODULE_NAME, L_WARN, format, args);
}

// 该函数船体给tirpc，释放内存空间使用的函数
void rpc_free(void *p, size_t n __attribute__ ((unused))) {
    try {
        free(p);
    } catch (...) {
        LOG(MODULE_NAME, L_ERROR,
            "Failed to free rpc memeory");
    }
}

// 该函数负责分配新的内存区域，并在分配失败的时候打印错误信息
void* rpc_malloc(size_t n, const char *file, int line, const char *function)
{
    void *p = malloc(n);
    if (p == NULL) {
        LOG(MODULE_NAME, L_ERROR,
            "Error occurred in %s in file %s:%d from %s",
            function, file, line, "gsh_malloc");
        abort();
    }
    return p;
}

// 包含对齐的内存空间分配函数
void* rpc_malloc_aligned(size_t a, size_t n,
                         const char *file, int line, const char *function)
{
    void *p;
    if (posix_memalign(&p, a, n) != 0)
        p = NULL;
    if (p == NULL) {
        LOG(MODULE_NAME, L_ERROR,
            "Error occurred in %s in file %s:%d from %s",
            function, file, line, "gsh_malloc_aligned");
        abort();
    }
    return p;
}

// 根据给定的结构体个数和结构体大小分配空间
void* rpc_calloc(size_t n, size_t s, const char *file, int line, const char *function)
{
    void *p = calloc(n, s);
    if (p == NULL) {
        LOG(MODULE_NAME, L_ERROR,
            "Error occurred in %s in file %s:%d from %s",
            function, file, line, "gsh_calloc");
        abort();
    }
    return p;
}

// 根据新的大小对之前已经分配的内存区域进行重新分配，新的区域会复制之前区域的数据
void* rpc_realloc(void *p, size_t n, const char *file, int line, const char *function)
{
    void *p2 = realloc(p, n);
    if (n != 0 && p2 == NULL) {
        LOG(MODULE_NAME, L_ERROR,
            "Error occurred in %s in file %s:%d from %s",
            function, file, line, "gsh_realloc");
        abort();
    }
    return p2;
}

/* 为一个新的dnfs请求申请空间并进行相关的初始化操作，其中xprt是服务端的传输
 * 句柄，存放了服务端的传输信息；xdrs则是XDR相关的句柄数据结构，内部存放的是
 * 相关的操作函数指针，并存放了xdr数据的指针和数据相关信息 */
struct svc_req *alloc_dnfs_request(SVCXPRT *xprt, XDR *xdrs) {
    /* 首先会为存放请求的结构化数据申请内存空间 */
    nfs_request_t *reqdata = reinterpret_cast<nfs_request_t *>(
            calloc(1, sizeof(nfs_request_t)));

    LOG_IF(reqdata == NULL, MODULE_NAME, EXIT_ERROR,
           "Failed to allocate memory for %s", __func__)
    LOG_IF(!xprt, MODULE_NAME, EXIT_ERROR, "Missing xprt for %s", __func__)
    LOG_IF(!xdrs, MODULE_NAME, EXIT_ERROR, "Missing xdrs for %s", __func__)

    LOG(MODULE_NAME, D_INFO, "%s: %p fd %d context %p",
        __func__, xprt, xprt->xp_fd, xdrs);

    /* 调用该函数表明当前存在一个针对xprt的引用，在进行相应请求的free操作的时候
     * 会调用SVC_RELEASE完成对应xprt数据的释放操作 */
    SVC_REF(xprt, SVC_REF_FLAG_NONE);

    reqdata->svc.rq_xprt = xprt;
    reqdata->svc.rq_xdrs = xdrs;
    reqdata->svc.rq_refcnt = 1;

    TAILQ_INIT_ENTRY(reqdata, dupes);

    /* 申请的结构体包含svc_req，但是返回的只能是svc_req的内容，通过这种方法
     * 可以存放更多的定制数，在最后释放内存的时候可以通过偏移量计算恢复原始结构体的地址 */
    return &reqdata->svc;
}

/* 从原始的地址数据中获取地址和端口号信息并转化为字符串表示 */
static string format_xprt_addr(SVCXPRT *xprt) {
    sockaddr_storage addr;
    const char *name = NULL;
    char ipname[SOCK_NAME_MAX];
    int port = 0;

    struct netbuf *phostaddr = svc_getcaller_netbuf(xprt);
    assert(phostaddr->len <= sizeof(sockaddr_storage) && phostaddr->buf != NULL);
    memcpy(&addr, phostaddr->buf, phostaddr->len);

    switch (addr.ss_family) {
        case AF_INET:
            name = inet_ntop(addr.ss_family,
                             &(((struct sockaddr_in *)&addr)->sin_addr),
                             ipname,
                             sizeof(ipname));
            port = ntohs(((struct sockaddr_in *)&addr)->sin_port);
            break;

        case AF_INET6:
            name = inet_ntop(addr.ss_family,
                             &(((struct sockaddr_in6 *)&addr)->sin6_addr),
                             ipname,
                             sizeof(ipname));
            port = ntohs(((struct sockaddr_in6 *)&addr)->sin6_port);
            break;

        case AF_LOCAL:
            return ((struct sockaddr_un *)&addr)->sun_path;
    }

    if (name == NULL) {
        return "<unknown>";
    } else {
        return string(name) + to_string(port);
    }
}

const char *xprt_stat_s[XPRT_DESTROYED + 1] = {
        "XPRT_IDLE",
        "XPRT_DISPATCH",
        "XPRT_DIED",
        "XPRT_DESTROYED"
};

/* 释放请求数据结构体对应的内存空间并执行其他销毁操作 */
void free_dnfs_request(struct svc_req *req, enum xprt_stat stat) {
    /* 尝试从svc_req结构体中恢复出完整的请求信息结构体内容，由于svc_req请求结构体在
     * nfs_request_t结构体内部，并且位置不确定，因此恢复需要通过结构体成员偏移量计算恢复出
     * 完整结构体的地址 */
    nfs_request_t *reqdata = get_parent_struct_addr(req, nfs_request_t, svc);
    SVCXPRT *xprt = reqdata->svc.rq_xprt;

    /* unlikely表示当前分支不太可能通过，从而指导CPU提高相应操作的处理速度
     * 这里必须确保相应的xprt句柄数据依然有效，无效句柄属于错误状态*/
    if (unlikely(stat > XPRT_DESTROYED)) {
        LOG(MODULE_NAME, L_ERROR, "SVC_DECODE on %p fd %d returned unknown %u",
            xprt, xprt->xp_fd, stat);
    } else {
        LOG(MODULE_NAME, D_INFO, "SVC_DECODE on %p fd %d (%s) xid=%" PRIu32
                " returned %s", xprt, xprt->xp_fd, format_xprt_addr(xprt).c_str(),
                reqdata->svc.rq_msg.rm_xid, xprt_stat_s[stat]);
    }

    LOG(MODULE_NAME, L_INFO, "%s: %p fd %d xp_refcnt %" PRIu32,
        __func__, xprt, xprt->xp_fd, xprt->xp_refcnt);

    free(reqdata);

    SVC_RELEASE(xprt, SVC_REF_FLAG_NONE);
}