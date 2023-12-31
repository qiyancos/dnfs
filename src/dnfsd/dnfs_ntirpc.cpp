/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

extern "C" {
#include "rpc/rpc.h"
#include "rpc/svc.h"
#include "rpc/svc_auth.h"
#include "rpc/svc_rqst.h"
#include "rpc/rpcb_clnt.h"
}

#include <cassert>
#include <sys/socket.h>
#include <sys/un.h>

#include "log/log.h"
#include "nfs/nfs_args.h"
#include "nfs/nfs_base.h"
#include "nfs/nfs_utils.h"
#include "utils/common_utils.h"
#include "utils/thread_utils.h"
#include "dnfsd/dnfs_ntirpc.h"
#include "dnfsd/dnfs_config.h"
#include "dnfsd/dnfs_auth.h"
#include "dnfsd/dnfs_exports.h"
#include "utils/net_utils.h"

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

/* 初始化ntirpc相关的基本参数配置 */
static int setup_ntirpc_params() {
    svc_init_params svc_params;
    int ix;
    int code;

    LOG(MODULE_NAME, L_INFO, "DNFS INIT: using TIRPC");

    memset(&svc_params, 0, sizeof(svc_params));

    /* 对rpc连接断开，创建连接分配请求资源和结束连接释放请求资源三个操作设置相应的callback函数 */
    svc_params.disconnect_cb = nullptr;
    svc_params.alloc_cb = alloc_dnfs_request;
    svc_params.free_cb = free_dnfs_request;

    /* 设置RPC的运行模式标签 */
    svc_params.flags = SVC_INIT_EPOLL;    /* use EPOLL event mgmt */
    svc_params.flags |= SVC_INIT_NOREG_XPRTS; /* don't call xprt_register */

    /* 能够同时存在的连接个数 */
    svc_params.max_connections = nfs_param.core_param.rpc.max_connections;
    svc_params.max_events = 1024;    /* length of epoll event queue */
    svc_params.ioq_send_max =
            nfs_param.core_param.rpc.max_send_buffer_size;
    svc_params.channels = N_EVENT_CHAN;
    svc_params.idle_timeout = nfs_param.core_param.rpc.idle_timeout_s;
    svc_params.ioq_thrd_min = nfs_param.core_param.rpc.ioq_thrd_min;
    svc_params.ioq_thrd_max = nfs_param.core_param.rpc.ioq_thrd_max;
    /* GSS ctx cache tuning, expiration */
    svc_params.gss_ctx_hash_partitions =
            nfs_param.core_param.rpc.gss.ctx_hash_partitions;
    svc_params.gss_max_ctx =
            nfs_param.core_param.rpc.gss.max_ctx;
    svc_params.gss_max_gc =
            nfs_param.core_param.rpc.gss.max_gc;

    /* Only after TI-RPC allocators, log channel are set up */
    if (!svc_init(&svc_params)) {
        LOG(MODULE_NAME, L_ERROR, "SVC initialization failed");
        return -1;
    }

    for (ix = 0; ix < EVCHAN_SIZE; ++ix) {
        rpc_evchan[ix].chan_id = 0;
        code = svc_rqst_new_evchan(&rpc_evchan[ix].chan_id,
                                   nullptr /* u_data */,
                                   SVC_RQST_FLAG_NONE);
        if (code) {
            LOG(MODULE_NAME, L_ERROR,
                "Cannot create TI-RPC event channel (%d, %d)", ix, code);
            return -1;
        }
    }

    return 0;
}

/* 注册tirpc的处理操作参数 */
void init_ntirpc_settings() {
    LOG(MODULE_NAME, L_INFO, "Start init ntirpc params");
    if (!tirpc_control(TIRPC_PUT_PARAMETERS, &ntirpc_pp)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Setting nTI-RPC parameters failed");
    }

    /* 初始化NTIRPC的基本参数 */
    if (setup_ntirpc_params()) {
        LOG(MODULE_NAME, EXIT_ERROR, "Failed to setup ntirpc params");
    }
}

// 用于ntirpc的警告信息输出和处理
void rpc_warnx(const char *format, ...) {
    va_list args;
    LOG(MODULE_NAME, L_WARN, format, args);
}

// 该函数传递给tirpc，释放内存空间使用的函数
void rpc_free(void *p, size_t n __attribute__ ((unused))) {
    try {
        gsh_free(p);
    } catch (...) {
        LOG(MODULE_NAME, L_ERROR,
            "Failed to free rpc memeory");
    }
}

// 该函数负责分配新的内存区域，并在分配失败的时候打印错误信息
void *rpc_malloc(size_t n, const char *file, int line, const char *function) {
    void *p = malloc(n);
    if (p == nullptr) {
        LOG(MODULE_NAME, L_ERROR,
            "Error occurred in %s in file %s:%d from %s",
            function, file, line, "gsh_malloc");
        abort();
    }
    return p;
}

// 包含对齐的内存空间分配函数
void *rpc_malloc_aligned(size_t a, size_t n,
                         const char *file, int line, const char *function) {
    void *p;
    if (posix_memalign(&p, a, n) != 0)
        p = nullptr;
    if (p == nullptr) {
        LOG(MODULE_NAME, L_ERROR,
            "Error occurred in %s in file %s:%d from %s",
            function, file, line, "gsh_malloc_aligned");
        abort();
    }
    return p;
}

// 根据给定的结构体个数和结构体大小分配空间
void *rpc_calloc(size_t n, size_t s, const char *file, int line, const char *function) {
    void *p = gsh_calloc(n, s);
    if (p == nullptr) {
        LOG(MODULE_NAME, L_ERROR,
            "Error occurred in %s in file %s:%d from %s",
            function, file, line, "gsh_calloc");
        abort();
    }
    return p;
}

// 根据新的大小对之前已经分配的内存区域进行重新分配，新的区域会复制之前区域的数据
void *rpc_realloc(void *p, size_t n, const char *file, int line, const char *function) {
    void *p2 = realloc(p, n);
    if (n != 0 && p2 == nullptr) {
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
    auto *reqdata = reinterpret_cast<nfs_request_t *>(
            gsh_calloc(1, sizeof(nfs_request_t)));

    LOG_IF(reqdata == nullptr, MODULE_NAME, EXIT_ERROR,
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
    sockaddr_storage addr{};
    struct netbuf *phostaddr = svc_getcaller_netbuf(xprt);
    assert(phostaddr->len <= sizeof(sockaddr_storage) && phostaddr->buf != nullptr);
    memcpy(&addr, phostaddr->buf, phostaddr->len);
    return format(&addr);
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

    gsh_free(reqdata);

    SVC_RELEASE(xprt, SVC_REF_FLAG_NONE);
}

/* 如果收到的RPC请求Program代码错误，调用该函数处理 */
enum xprt_stat nfs_rpc_noprog(nfs_request_t *reqdata) {
    LOG(MODULE_NAME, D_ERROR,
        "Invalid Program number %" PRIu32,
        reqdata->svc.rq_msg.cb_prog);
    return svcerr_noprog(&reqdata->svc);
}

static enum nfs_req_result complete_request(
        nfs_request_t *reqdata, enum nfs_req_result rc) {
    SVCXPRT *xprt = reqdata->svc.rq_xprt;
    const nfs_function_desc_t *reqdesc = reqdata->funcdesc;

    /* If request is dropped, no return to the client */
    if (rc == NFS_REQ_DROP) {
        /* The request was dropped */
        LOG(MODULE_NAME, D_INFO,
            "Drop request rpc_xid=%" PRIu32
                    ", program %" PRIu32
                    ", version %" PRIu32
                    ", function %" PRIu32,
            reqdata->svc.rq_msg.rm_xid,
            reqdata->svc.rq_msg.cb_prog,
            reqdata->svc.rq_msg.cb_vers,
            reqdata->svc.rq_msg.cb_proc);
        return rc;
    }

    LOG(MODULE_NAME, D_INFO,
        "Before svc_sendreply on socket %d", xprt->xp_fd);

    reqdata->svc.rq_msg.RPCM_ack.ar_results.where = reqdata->res_nfs;
    reqdata->svc.rq_msg.RPCM_ack.ar_results.proc =
            reqdesc->xdr_encode_func;

    if (svc_sendreply(&reqdata->svc) >= XPRT_DIED) {
        LOG(MODULE_NAME, D_INFO,
            "NFS DISPATCHER: FAILURE: Error while calling svc_sendreply on"
            " a new request. rpcxid=%" PRIu32
                    " socket=%d function:%s program:%" PRIu32
                    " nfs version:%" PRIu32
                    " proc:%" PRIu32
                    " errno: %d",
            reqdata->svc.rq_msg.rm_xid,
            xprt->xp_fd,
            reqdesc->funcname,
            reqdata->svc.rq_msg.cb_prog,
            reqdata->svc.rq_msg.cb_vers,
            reqdata->svc.rq_msg.cb_proc,
            errno);
        SVC_DESTROY(xprt);
        rc = NFS_REQ_XPRT_DIED;
    }

    LOG(MODULE_NAME, D_INFO,
        "After svc_sendreply on socket %d", xprt->xp_fd);

    return rc;
}

void free_args(nfs_request_t *reqdata) {
    const nfs_function_desc_t *reqdesc = reqdata->funcdesc;

    /* Free the allocated resources once the work is done */
    /* Free the arguments */
    if ((reqdata->svc.rq_msg.cb_vers == 2)
        || (reqdata->svc.rq_msg.cb_vers == 3)
        || (reqdata->svc.rq_msg.cb_vers == 4)) {
        if (!xdr_free(reqdesc->xdr_decode_func,
                      &reqdata->arg_nfs)) {
            LOG(MODULE_NAME, L_ERROR,
                "%s FAILURE: Bad xdr_free for %s",
                __func__,
                reqdesc->funcname);
        }
    }

    /* Finalize the request. */
    nfs_dupreq_rele(reqdata);
}

void complete_request_instrumentation(nfs_request_t *reqdata)
{
#ifdef USE_LTTNG
    tracepoint(nfs_rpc, op_end, reqdata);
#endif
}


void nfs_rpc_complete_async_request(nfs_request_t *reqdata,
                                    enum nfs_req_result rc)
{
    complete_request_instrumentation(reqdata);
    rc = complete_request(reqdata, rc);
    free_args(reqdata);
}

/* RPC处理主程序入口 */
enum xprt_stat nfs_rpc_process_request(nfs_request_t *reqdata, bool retry) {
    LOG(MODULE_NAME, D_INFO,
        "=============================Service call interface %s=============================",
        reqdata->funcdesc->funcname);
    const nfs_function_desc_t *reqdesc = reqdata->funcdesc;
    nfs_arg_t *arg_nfs = &reqdata->arg_nfs;
    SVCXPRT *xprt = reqdata->svc.rq_xprt;
    XDR *xdrs = reqdata->svc.rq_xdrs;
    enum auth_stat auth_rc;
    int rc = NFS_REQ_OK;
    dupreq_status_t dpq_status;
    bool no_dispatch = false;

    if (retry)
        goto retry_after_drc_suspend;

    /* If req is uncacheable, or if req is v41+, nfs_dupreq_start will do
     * nothing but allocate a result object and mark the request (ie, the
     * path is short, lockless, and does no hash/search). */
    dpq_status = nfs_dupreq_start(reqdata);
    /*判断是否成功申请内存*/
    if (dpq_status == DUPREQ_SUCCESS) {
        /* A new request, continue processing it. */
        LOG(MODULE_NAME, L_INFO,
            "Requested memory successfully");
    } else {
        goto freeargs;
    }

    LOG(MODULE_NAME, D_INFO,
        "About to authenticate Prog=%" PRIu32
                ", vers=%" PRIu32 ", proc=%" PRIu32
                ", xid=%" PRIu32 ", SVCXPRT=%p, fd=%d",
        reqdata->svc.rq_msg.cb_prog,
        reqdata->svc.rq_msg.cb_vers,
        reqdata->svc.rq_msg.cb_proc,
        reqdata->svc.rq_msg.rm_xid,
        xprt, xprt->xp_fd);

    /* If authentication is AUTH_NONE or AUTH_UNIX, then the value of
     * no_dispatch remains false and the request proceeds normally.
     *
     * If authentication is RPCSEC_GSS, no_dispatch may have value true,
     * this means that gc->gc_proc != RPCSEC_GSS_DATA and that the message
     * is in fact an internal negotiation message from RPCSEC_GSS using
     * GSSAPI. It should not be processed by the worker and SVC_STAT
     * should be returned to the dispatcher.
     */
    auth_rc = svc_auth_authenticate(&reqdata->svc, &no_dispatch);
    if (auth_rc != AUTH_OK) {
        LOG(MODULE_NAME, L_INFO,
            "Could not authenticate request... rejecting with AUTH_STAT=%s",
            auth_stat2str(auth_rc));
        return svcerr_auth(&reqdata->svc, auth_rc);
    }

    /*
     * Extract RPC argument.
     */
    LOG(MODULE_NAME, D_INFO,
        "Before SVCAUTH_CHECKSUM on SVCXPRT %p fd %d",
        xprt, xprt->xp_fd);

    memset(arg_nfs, 0, sizeof(nfs_arg_t));
    reqdata->svc.rq_msg.rm_xdr.where = arg_nfs;
    reqdata->svc.rq_msg.rm_xdr.proc = reqdesc->xdr_decode_func;
    xdrs->x_public = &reqdata->lookahead;

    if (!SVCAUTH_CHECKSUM(&reqdata->svc)) {
        LOG(MODULE_NAME, D_INFO,
            "SVCAUTH_CHECKSUM failed for Program %" PRIu32
                    ", Version %" PRIu32
                    ", Function %" PRIu32
                    ", xid=%" PRIu32
                    ", SVCXPRT=%p, fd=%d",
            reqdata->svc.rq_msg.cb_prog,
            reqdata->svc.rq_msg.cb_vers,
            reqdata->svc.rq_msg.cb_proc,
            reqdata->svc.rq_msg.rm_xid,
            xprt, xprt->xp_fd);

        if (!xdr_free(reqdesc->xdr_decode_func, arg_nfs)) {
            LOG(MODULE_NAME, L_ERROR,
                "%s FAILURE: Bad xdr_free for %s",
                __func__, reqdesc->funcname);
        }
        return svcerr_decode(&reqdata->svc);
    }

    /* Set up initial export permissions that don't allow anything. */
    export_check_access();

    retry_after_drc_suspend:
    /* If we come here on a retry after drc suspend, then we already did
     * the stuff above.
     */

    /* Don't waste time for null or invalid ops
     * null op code in all valid protos == 0
     * and invalid protos all point to invalid_funcdesc
     * NFS v2 is set to invalid_funcdesc in nfs_rpc_get_funcdesc()
     */
    if (reqdesc != &invalid_funcdesc
        and reqdata->svc.rq_msg.cb_proc != NFSPROC_NULL) {

        rc = reqdesc->service_function(arg_nfs, &reqdata->svc,
                                       reqdata->res_nfs);

        if (rc == NFS_REQ_ASYNC_WAIT) {
            /* The request is suspended, don't touch the request in
             * any way because the resume may already be scheduled
             * and running on nother thread. The xp_resume_cb has
             * already been set up before we started processing
             * ops on this request at all.
             */
            return XPRT_SUSPEND;
        }
    }
    complete_request(reqdata, static_cast<nfs_req_result>(rc));

    freeargs:
    free_args(reqdata);

    LOG(MODULE_NAME, D_INFO,
        "=============================Service already reply interface %s=============================",
        reqdata->funcdesc->funcname);
    return SVC_STAT(xprt);
}

/* 如果处理协议存在但是相应的函数不存在，那么会执行该函数处理错误 */
enum xprt_stat nfs_rpc_noproc(nfs_request_t *reqdata) {
    LOG(MODULE_NAME, D_ERROR,
        "Invalid Procedure %" PRIu32
                " in protocol Version %" PRIu32
                " for Program number %" PRIu32,
        reqdata->svc.rq_msg.cb_proc,
        reqdata->svc.rq_msg.cb_vers,
        reqdata->svc.rq_msg.cb_prog);
    return svcerr_noproc(&reqdata->svc);
}

/* 如果出现了一个不支持的协议版本，那么会调用该函数处理 */
enum xprt_stat nfs_rpc_novers(nfs_request_t *reqdata) {
    LOG(MODULE_NAME, D_ERROR,
        "Invalid protocol Version %" PRIu32
                " for Program number %" PRIu32,
        reqdata->svc.rq_msg.cb_vers,
        reqdata->svc.rq_msg.cb_prog);
    return svcerr_progvers(&reqdata->svc, NFS_V3, NFS_V3);
}
