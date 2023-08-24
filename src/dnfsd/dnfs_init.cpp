/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 * Contributor(nfs-ganesha): Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
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
#include "rpc/svc.h"
#include "rpc/svc_rqst.h"
#include "rpc/rpcb_clnt.h"
}

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <netinet/tcp.h>

#include <exception>
#include <experimental/filesystem>

#include "nfs/nfs23.h"
#include "nfs/nfsv41.h"
#include "log/log.h"
#include "utils/common_utils.h"
#include "dnfsd/dnfs_config.h"
#include "dnfsd/dnfs_init.h"
#include "dnfsd/dnfs_ntirpc.h"

using namespace std;

#define MODULE_NAME "main"

/* 默认配置文件的字符表达形式 */
static const char default_config[] =
        "# default config file content\n"
        "\n";

/* 创建一个默认初始化的配置 */
static void init_default_config(const string& config_file_path) {
    string config_dir = config_file_path.substr(0,
            config_file_path.find_last_of('/'));
    try {
        experimental::filesystem::create_directories(config_dir);
    } catch (exception& e) {
        fprintf(stderr,
                "Failed to create directory \"%s\" for config file: %s.\n",
                config_dir.c_str(), e.what());
        exit(-1);
    }

    ofstream config_file_os;
    try {
        config_file_os.open(config_file_path.c_str(), ios::out);
    } catch (exception& e) {
        fprintf(stderr,
                "Failed to create default config file \"%s\"",
                config_file_path.c_str());
        exit(-1);
    }
    config_file_os << default_config;
    config_file_os.close();
}

/* 初始化配置文件并进行解析 */
void init_config(const string& config_file_path) {
    if (access(config_file_path.c_str(), F_OK) == -1) {
        /* 如果目标文件不存在，创建一个默认初始化的配置 */
        init_default_config(config_file_path);
    }
    try {
        dnfs_config = YAML::LoadFile(config_file_path);
    } catch (exception& e) {
        fprintf(stderr, "Failed to load config file %s: %s\n",
                config_file_path.c_str(), e.what());
        exit(-1);
    }
    /* 这里还会做一些基本配置文件的校验 */
    /*TODO*/
}


/* 初始化日志相关的配置 */
void init_logging(const string& exec_name, const string& nfs_host_name,
                  const log_level_t debug_level, const bool detach_flag,
                  const string& arg_log_path) {
    /* 日志路径 */
    string log_path = arg_log_path;
    /*初始化日志管理器*/
    logger.init(exec_name, nfs_host_name);
    /* 初始化主程序日志模块 */
    logger.init_module(MODULE_NAME);
    logger.init_module("config");
    logger.init_module("rpc");
    logger.init_module("thread_pool");
    /*初始化日志等级*/
    logger.set_log_level(debug_level);

    string temp;
    if (config_get(temp, dnfs_config, {"log", "path"})) {
        log_path = temp;
    }

    char buffer[256] = "";
    size_t buf_size = 0;
    /*初始化日志文件信息*/
    /* TODO 这里需要对参数进行校验避免buffer overflow */
    if (config_get(temp, dnfs_config, {"log", "limit_type"})) {
        buf_size += sprintf(buffer, "@(%s", temp.c_str());
        if (config_get(temp, dnfs_config, {"log", "limit_info"})) {
            buf_size += sprintf(buffer, ",%s", temp.c_str());
        }
        if (config_get(temp, dnfs_config, {"log", "backup_count"})) {
            buf_size += sprintf(buffer, ",%s", temp.c_str());
        }
        buffer[buf_size] = ')';
    }
    log_path += buffer;
    if (!detach_flag) {
        /* 只检查log_path部分的合法性 */
        if (logger.set_log_output(L_INFO, log_path + ":stdout:syslog", &temp)) {
            fprintf(stderr, "%s", temp.c_str());
            exit(-1);
        }
        logger.set_log_output({EXIT_ERROR, L_ERROR, L_WARN},
                              log_path + ":stderr:syslog", nullptr);
        logger.set_log_output(D_INFO, log_path + ":stdout", nullptr);
        logger.set_log_output({D_ERROR, D_WARN}, log_path += ":stderr", nullptr);
    } else {
        /* 只检查log_path部分的合法性 */
        if (logger.set_log_output(log_path, &temp)) {
            fprintf(stderr, "%s", temp.c_str());
            exit(-1);
        }
        logger.set_log_output(L_INFO, log_path + ":syslog", nullptr);
        logger.set_log_output({EXIT_ERROR, L_ERROR, L_WARN},
                              log_path + ":syslog", nullptr);
    }
    if (config_get(temp, dnfs_config, {"log", "formatter"})) {
        string error_info;
        if (logger.set_formatter(temp, &error_info)) {
            fprintf(stderr, "%s", temp.c_str());
            exit(-1);
        }
    }
    logger.set_default_attr_from(MODULE_NAME, nullptr);
}

/* 崩溃信号默认处理函数 */
static void crash_handler(int signo, [[maybe_unused]] siginfo_t *info,
                          [[maybe_unused]] void *ctx) {
    LOG("Crash Handler", L_BACKTRACE, "");
    /* re-raise the signal for the default signal handler to dump core */
    raise(signo);
}

/* 将处理函数挂载到对应的信号处理上 */
static void install_sighandler(int signo,
                               void (*handler)(int, siginfo_t *, void *)) {
    struct sigaction sa = {};
    int ret;

    sa.sa_sigaction = handler;
    /* set SA_RESETHAND to restore default handler */
    /* SA_SIGINFO：使用更加详细的handler类型，传递三个参数而不是一个，获取更多的信息 */
    /* SA_RESETHAND：在执行过一次信号对应的处理函数后，重置handler为缺省的处理函数 */
    /* SA_NODEFER：执行处理函数的时候不会阻塞信号，即可以持续接收新的信号 */
    sa.sa_flags = SA_SIGINFO | SA_RESETHAND | SA_NODEFER;

    sigemptyset(&sa.sa_mask);

    /* sigaction函数用于改变进程接收到特定信号后的行为。该函数的第一个参数为信号的值，
     * 可以为除SIGKILL及SIGSTOP外的任何一个特定有效的信号（为这两个信号定义自己的处理函数，
     * 将导致信号安装错误）。第二个参数是指向结构sigaction的一个实例的指针，
     * 在结构sigaction的实例中，指定了对特定信号的处理，可以为空，进程会以缺省方式对信号处理；
     * 第三个参数oldact指向的对象用来保存原来对相应信号的处理，可指定oldact为NULL。*/
    ret = sigaction(signo, &sa, NULL);
    if (ret) {
        LOG(MODULE_NAME, L_WARN,
                "Install handler for signal (%s) failed",
                strsignal(signo));
    }
}

/* 初始化错误信号的处理函数 */
void init_crash_handlers(void) {
    install_sighandler(SIGSEGV, crash_handler);
    install_sighandler(SIGABRT, crash_handler);
    install_sighandler(SIGBUS, crash_handler);
    install_sighandler(SIGILL, crash_handler);
    install_sighandler(SIGFPE, crash_handler);
    install_sighandler(SIGQUIT, crash_handler);
}

/* 检查malloc功能的可用性 */
void init_check_malloc() {
    /* Check malloc(0) - DNFS assumes malloc(0) returns non-NULL pointer.
     * Note we use malloc and calloc directly here and not gsh_malloc and
     * gsh_calloc because we don't want those functions to abort(), we
     * want to log a descriptive message.
     */
    void *p;

    p = malloc(0);
    if (p == NULL) {
        LOG(MODULE_NAME, EXIT_ERROR,
                 "DNFS's assumption that malloc(0) returns a non-NULL pointer"
                 " is not true, Ganesha can not work with the memory allocator in use. Aborting.");
    }
    free(p);

    p = calloc(0, 0);
    if (p == NULL) {
        LOG(MODULE_NAME, EXIT_ERROR,
                 "Ganesha's assumption that calloc(0, 0) returns a non-NULL pointer"
                 " is not true, Ganesha can not work with the memory allocator in use. Aborting.");
    }
    free(p);
}

/* 初始化线程对信号的处理操作 */
int init_thread_signal_mask() {
    /* 解除文件大小资源限制，对相关的停止信号进行忽略 */
    signal(SIGXFSZ, SIG_IGN);
    /* 在所有默认子线程中阻塞指定的信号以便专门的信号处理线程能够正确处理信号 */
    sigset_t signals_to_block;
    sigemptyset(&signals_to_block);
    /* SIGTERM 终止进程 软件终止信号 */
    sigaddset(&signals_to_block, SIGTERM);
    /* SIGHUP 终止进程 终端线路挂断 */
    sigaddset(&signals_to_block, SIGHUP);
    /* SIGPIPE 终止进程 向一个没有读进程的管道写数据 */
    sigaddset(&signals_to_block, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &signals_to_block, NULL) != 0) {
        LOG(MODULE_NAME, EXIT_ERROR,
                 "Could not start nfs daemon, pthread_sigmask failed");
        return -1;
    }
    return 0;
}

/**
 * TI-RPC event channels.  Each channel is a thread servicing an event
 * demultiplexer.
 */
struct rpc_evchan {
    uint32_t chan_id;	/*< Channel ID */
};

static struct rpc_evchan rpc_evchan[EVCHAN_SIZE];
int udp_socket;
int tcp_socket;

/* 为已经分配的套接字设置相关的属性信息 */
static int alloc_socket_setopts() {
    int one = 1;
    const struct nfs_core_param *nfs_cp = &nfs_param.core_param;

    /* Use SO_REUSEADDR in order to avoid wait the 2MSL timeout */
    if (setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR,
                   &one, sizeof(one))) {
        LOG(MODULE_NAME, L_ERROR,
            "Bad udp socket options reuseaddr for NFS_V3 udp sock, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }

    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR,
                   &one, sizeof(one))) {
        LOG(MODULE_NAME, L_ERROR,
            "Bad tcp socket option reuseaddr for NFS_V3 tcp sock, error %d(%s)",
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

    /* We prefer using non-blocking socket in the specific case */
    if (fcntl(udp_socket, F_SETFL, FNDELAY) == -1) {
        LOG(MODULE_NAME, L_ERROR,
            "Cannot set udp socket for %s as non blocking, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }

    return 0;
}

/* 为DNFS分配socket套接字 */
static int allocate_sockets(void) {
    LOG(MODULE_NAME, D_INFO, "Allocation of the sockets");

    udp_socket = tcp_socket = -1;

    /* 首先为NFSV3分配UDP的监听接口 */
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (udp_socket == -1) {
        if (errno == EAFNOSUPPORT) {
            LOG(MODULE_NAME, L_ERROR, "No V4 intfs configured?!");
        }

        LOG(MODULE_NAME, L_ERROR,
            "Cannot allocate a udp socket for NFS_V3, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }

    /* 首先为NFSV3分配TCP的监听接口 */
    tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (tcp_socket == -1) {
        LOG(MODULE_NAME, L_ERROR,
            "Cannot allocate a tcp socket for NFS_V3, error %d(%s)",
            errno, strerror(errno));
        return -1;
    }
    return 0;
}

/* 初始化ntirpc相关的基本参数配置 */
static int setup_ntirpc_params() {
    svc_init_params svc_params;
    int ix;
    int code;

    LOG(MODULE_NAME, L_INFO, "DNFS INIT: using TIRPC");

    memset(&svc_params, 0, sizeof(svc_params));

    /* 对rpc连接断开，创建连接分配请求资源和结束连接释放请求资源三个操作设置相应的callback函数 */
    svc_params.disconnect_cb = NULL;
    svc_params.alloc_cb = alloc_dnfs_request;
    svc_params.free_cb = free_dnfs_request;

    /* 设置RPC的运行模式标签 */
    svc_params.flags = SVC_INIT_EPOLL;	/* use EPOLL event mgmt */
    svc_params.flags |= SVC_INIT_NOREG_XPRTS; /* don't call xprt_register */

    /* 能够同时存在的连接个数 */
    svc_params.max_connections = nfs_param.core_param.rpc.max_connections;
    svc_params.max_events = 1024;	/* length of epoll event queue */
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
                                   NULL /* u_data */,
                                   SVC_RQST_FLAG_NONE);
        if (code) {
            LOG(MODULE_NAME, L_ERROR,
                "Cannot create TI-RPC event channel (%d, %d)", ix, code);
            return -1;
        }
    }

    return 0;
}

/* NFSV3的socket绑定信息 */
proto_data nfsv3_sock_info;

/* 将已经申请到的套接字绑定到指定的端口号上 */
static void bind_udp_sockets() {
    LOG(MODULE_NAME, D_INFO, "Binding to UDP address %s",
        format(nfs_param.core_param.bind_addr).c_str());

    proto_data *pdatap = &nfsv3_sock_info;

    memset(&pdatap->sinaddr_udp, 0,
           sizeof(pdatap->sinaddr_udp));

    pdatap->sinaddr_udp.sin_family = AF_INET;

    /* all interfaces */
    pdatap->sinaddr_udp.sin_addr.s_addr =
            ((struct sockaddr_in *)
                    &nfs_param.core_param.bind_addr)->
                    sin_addr.s_addr;
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

    /* 从socket中获取ntirpc需要的信息并存储起来 */
    if (!__rpc_fd2sockinfo(udp_socket,
                           &pdatap->si_udp6)) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Cannot get socket info for udp6 socket errno=%d (%s)",
            errno, strerror(errno));
    }

    LOG(MODULE_NAME, D_INFO, "Binding UDP socket to address %s",
        format(*((sockaddr_storage*)pdatap->bindaddr_udp6.addr.buf)).c_str());

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

/* 将已经申请到的套接字绑定到指定的端口号上 */
static void bind_tcp_sockets() {
    LOG(MODULE_NAME, D_INFO, "Binding to TCP address %s",
        format(nfs_param.core_param.bind_addr).c_str());

    proto_data *pdatap = &nfsv3_sock_info;

    memset(&pdatap->sinaddr_tcp, 0,
           sizeof(pdatap->sinaddr_tcp));
    pdatap->sinaddr_tcp.sin_family = AF_INET;
    /* all interfaces */
    pdatap->sinaddr_tcp.sin_addr.s_addr =
            ((struct sockaddr_in *)
                    &nfs_param.core_param.bind_addr)->sin_addr.s_addr;
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
        format(*((sockaddr_storage*)pdatap->bindaddr_tcp6.addr.buf)).c_str());

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

/* 使用rpcbind取消当前rpc的绑定关系，以便绑定新的内容 */
static void unregister_rpc(void)
{
    const rpcprog_t& prog = nfs_param.core_param.program;
    const rpcvers_t vers1 = NFS_V3;
    const rpcvers_t vers2 = NFS_V4;

    rpcvers_t vers;

    for (vers = vers1; vers <= vers2; vers++) {
        rpcb_unset(prog, vers, netconfig_udpv4);
        rpcb_unset(prog, vers, netconfig_tcpv4);
    }
}

/**
 * @brief xprt destructor callout
 *
 * @param[in] xprt Transport to destroy
 */
static enum xprt_stat nfs_rpc_free_user_data(SVCXPRT *xprt)
{
    if (xprt->xp_u2) {
        /*TODO*/
//        nfs_dupreq_put_drc(xprt->xp_u2);
//        xprt->xp_u2 = NULL;
    }
    return XPRT_DESTROYED;
}

SVCXPRT *udp_xprt;
SVCXPRT *tcp_xprt;

/* 给每一个协议创建相应的svcxprt网络传输句柄，每一个协议对应的每一个网络协议都
 * 有一个单独的XPRT传输句柄，用来执行后续的处理操作 */
void create_svcxprts() {
    /* 创建UDP相关的XPRT */
    udp_xprt = svc_dg_create(udp_socket,
                             nfs_param.core_param.rpc.max_send_buffer_size,
                             nfs_param.core_param.rpc.max_recv_buffer_size);

    if (udp_xprt == NULL) {
        LOG(MODULE_NAME, EXIT_ERROR, "Cannot allocate UDP SVCXPRT");
    }

    udp_xprt->xp_dispatch.rendezvous_cb = nfs_rpc_dispatch_udp_NFS;

    /* Hook xp_free_user_data (finalize/free private data) */
    (void)SVC_CONTROL(udp_xprt, SVCSET_XP_FREE_USER_DATA,
                      (void *)nfs_rpc_free_user_data);

    (void)svc_rqst_evchan_reg(rpc_evchan[UDP_UREG_CHAN].chan_id,
                              udp_xprt,
                              SVC_RQST_FLAG_XPRT_UREG);

    /* 创建TCP相关的XPRT */
    tcp_xprt = svc_vc_ncreatef(tcp_socket,
                            nfs_param.core_param.rpc.max_send_buffer_size,
                            nfs_param.core_param.rpc.max_recv_buffer_size,
                            SVC_CREATE_FLAG_CLOSE | SVC_CREATE_FLAG_LISTEN);
    if (tcp_xprt == NULL) {
        LOG(MODULE_NAME, EXIT_ERROR, "Cannot allocate TCP SVCXPRT");
    }

    tcp_xprt->xp_dispatch.rendezvous_cb = nfs_rpc_dispatch_tcp_NFS;

    /* Hook xp_free_user_data (finalize/free private data) */
    (void)SVC_CONTROL(tcp_xprt, SVCSET_XP_FREE_USER_DATA,
                      (void*) nfs_rpc_free_user_data);

    (void)svc_rqst_evchan_reg(rpc_evchan[TCP_UREG_CHAN].chan_id,
                              tcp_xprt, SVC_RQST_FLAG_XPRT_UREG);
}

/* 调用rpcbind服务绑定当前的函数到RPC调用操作上 */
static void register_rpc_program() {
    LOG(MODULE_NAME, D_INFO, "Registering V3/UDP");

    /* XXXX fix svc_register! */
    if (!svc_reg(udp_xprt, nfs_param.core_param.program, (u_long) NFS_V3,
            nfs_rpc_dispatch_dummy, netconfig_udpv4)) {
        LOG(MODULE_NAME, EXIT_ERROR, "Cannot register V%d on UDP", (int)NFS_V3);
    }

    LOG(MODULE_NAME, D_INFO, "Registering V%d/TCP", (int)NFS_V3);

    if (!svc_reg(tcp_xprt, nfs_param.core_param.program, (u_long) NFS_V3,
            nfs_rpc_dispatch_dummy, netconfig_tcpv4)) {
        LOG(MODULE_NAME, EXIT_ERROR, "Cannot register %s V%d on TCP", (int)NFS_V3);
    }
}

/* 初始化nfs服务相关的接口注册操作 */
static void dnfs_init_svc(void) {
    /* 初始化NTIRPC的基本参数 */
    if (setup_ntirpc_params()) {
        LOG(MODULE_NAME, EXIT_ERROR, "Failed to setup ntirpc params");
    }

    /* 为NFS_V3的协议分配UDP和TCP的套接字 */
    if (allocate_sockets()) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Failed to allocate socket for NFS_V3, error %d(%s)",
            errno, strerror(errno));
    }

    LOG(MODULE_NAME, D_INFO, "Socket numbers are: tcp=%d udp=%d",
        tcp_socket, udp_socket);

    /* 对分配的socket的属性进行设置 */
    if (alloc_socket_setopts()) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Error setting socket option for NFS_V3");
    }

    /* 将生成的套接字socket绑定到指定的端口号 */
    bind_udp_sockets();
    bind_tcp_sockets();
    LOG(MODULE_NAME, L_INFO, "Bind sockets successful");

    /* 从portmapper/rpcbind中取消当前已有rpc程序的绑定关系以便进行新的绑定 */
    unregister_rpc();

    /* 初始化网络传输的xprt句柄 */
    create_svcxprts();

    /* 对RPC的程序进行正式的注册，包括UDP和TCP，一旦完成注册，就可以开始接受rpc请求了 */
    register_rpc_program();
}

/* dnfs启动处理函数 */
void dnfs_start() {
    if (nfs_start_info.dump_default_config) {
        dump_config();
        exit(0);
    }

    /* Make sure DNFS runs with a 0000 umask. */
    umask(0000);

    /* RPC Initialisation - exits on failure */
    dnfs_init_svc();

    LOG(MODULE_NAME, L_INFO,
        "-------------------------------------------------");
    LOG(MODULE_NAME, L_INFO, "             NFS SERVER INITIALIZED");
    LOG(MODULE_NAME, L_INFO,
        "-------------------------------------------------");
}
