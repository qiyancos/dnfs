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

#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "log/log.h"
#include "log/log_exception.h"
#include "utils/common_utils.h"
#include "utils/thread_utils.h"
#include "dnfsd/dnfsd.h"
#include "dnfsd/dnfsd_exit.h"
#include "dnfsd/dnfs_config.h"
#include "dnfsd/dnfs_init.h"
#include "dnfsd/dnfs_signal_proc.h"
#include "nfs/nfs_init.h"
#include "mnt/mnt_init.h"

using namespace std;

#define MODULE_NAME "main"

/* 初始化日志相关的配置 */
void init_logging(const string &exec_name, const string &nfs_host_name,
                  const log_level_t debug_level, const bool detach_flag,
                  const string &arg_log_path) {
    fprintf(stdout, "Start init logging setup\n");
    const dnfs_logging_config &log_config = dnfs_config.log_config;
    /* 日志路径 */
    string log_path;
    if (!arg_log_path.empty()) {
        log_path = dnfs_config.log_config.path = arg_log_path;
    } else {
        log_path = dnfs_config.log_config.path;
    }
    /*初始化日志管理器*/
    logger.init(exec_name, nfs_host_name);
    /* 初始化主程序日志模块 */
    logger.init_module(MODULE_NAME);
    logger.init_module("config");
    logger.init_module("rpc");
    logger.init_module("net");
    logger.init_module("thread_pool");
    logger.init_module("DNFS");
    logger.init_module("NFS");
    logger.init_module("MNT");
    /*初始化日志等级*/
    logger.set_log_level(debug_level);

    if (!log_path.empty()) {
        log_path = format_message("%s@(%s,%s,%d)", log_config.path.c_str(),
                                  log_config.limit_type.c_str(),
                                  log_config.limit_info.c_str(),
                                  log_config.backup_count);
    }

    if (!detach_flag) {
        /* 只检查log_path部分的合法性 */
        try {
            logger.set_log_output(L_INFO, log_path + ":stdout:syslog");
        } catch (LogException &e) {
            fprintf(stderr, "%s\n", e.what());
            exit_process(-1);
        }
        logger.set_log_output({EXIT_ERROR, L_ERROR, L_WARN},
                              log_path + ":stderr:syslog");
        logger.set_log_output(D_INFO, log_path + ":stdout");
        logger.set_log_output({D_ERROR, D_WARN}, log_path += ":stderr");
    } else {
        /* 只检查log_path部分的合法性 */
        try {
            logger.set_log_output(log_path);
        } catch (LogException &e) {
            fprintf(stderr, "%s\n", e.what());
            exit_process(-1);
        }
        logger.set_log_output(L_INFO, log_path + ":syslog");
        logger.set_log_output({EXIT_ERROR, L_ERROR, L_WARN},
                              log_path + ":syslog");
    }
    try {
        logger.set_formatter(log_config.formatter);
    } catch (LogException &e) {
        fprintf(stderr, "%s\n", e.what());
        exit_process(-1);
    }
    logger.set_default_attr_from(MODULE_NAME);
}

/* 初始化错误信号的处理函数 */
void init_crash_handlers() {
    LOG(MODULE_NAME, L_INFO, "Start init crash handler for main thread");
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
    if (p == nullptr) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "DNFS's assumption that malloc(0) returns a non-NULL pointer"
            " is not true, Ganesha can not work with the memory allocator in use. Aborting.");
    }
    gsh_free(p);

    p = calloc(0, 0);
    if (p == nullptr) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Ganesha's assumption that calloc(0, 0) returns a non-NULL pointer"
            " is not true, Ganesha can not work with the memory allocator in use. Aborting.");
    }
    gsh_free(p);
}

/* 初始化线程对信号的处理操作 */
int init_thread_signal_mask() {
    LOG(MODULE_NAME, L_INFO, "Start init thread signal mask");
    /* 解除文件大小资源限制，对相关的停止信号进行忽略 */
    signal(SIGXFSZ, SIG_IGN);
    /* 在所有默认子线程中阻塞指定的信号以便专门的信号处理线程能够正确处理信号 */
    sigset_t signals_to_block;
    sigemptyset(&signals_to_block);
    /* SIGTERM 终止进程 软件终止信号 */
    sigaddset(&signals_to_block, SIGINT);
    /* SIGTERM 终止进程 软件终止信号 */
    sigaddset(&signals_to_block, SIGTERM);
    /* SIGHUP 终止进程 终端线路挂断 */
    sigaddset(&signals_to_block, SIGHUP);
    /* SIGPIPE 终止进程 向一个没有读进程的管道写数据 */
    sigaddset(&signals_to_block, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &signals_to_block, nullptr) != 0) {
        LOG(MODULE_NAME, EXIT_ERROR,
            "Could not start nfs daemon, pthread_sigmask failed");
        return -1;
    }
    return 0;
}

struct netconfig *netconfig_udpv4;
struct netconfig *netconfig_tcpv4;

/* 初始化nfs服务相关的接口注册操作 */
static void dnfs_init_svc() {
    /* Get the netconfig entries from /etc/netconfig */
    netconfig_udpv4 = (struct netconfig *) getnetconfigent("udp");
    if (netconfig_udpv4 == nullptr)
        LOG(MODULE_NAME, L_ERROR,
            "Cannot get udp netconfig, cannot get an entry for udp in netconfig file. Check file /etc/netconfig...");

    /* Get the netconfig entries from /etc/netconfig */
    netconfig_tcpv4 = (struct netconfig *) getnetconfigent("tcp");
    if (netconfig_tcpv4 == nullptr)
        LOG(MODULE_NAME, L_ERROR,
            "Cannot get tcp netconfig, cannot get an entry for tcp in netconfig file. Check file /etc/netconfig...");


    /* 初始化默认运行配置中的绑定地址数据 */
    if (!inet_pton(nfs_param.core_param.bind_addr.sin_family,
                   nfs_param.core_param.bind_addr_str.c_str(),
                   &nfs_param.core_param.bind_addr.sin_addr)) {
        LOG(MODULE_NAME, EXIT_ERROR, "Illegal bing ipv4 addr \"%s\" for nfsv3",
            nfs_param.core_param.bind_addr_str.c_str());
    }

    /*获取verifier*/
    /* Set the server's boot time and epoch */
    now(&nfs_ServerBootTime);
    nfs_ServerEpoch = (time_t) nfs_ServerBootTime.tv_sec;

    union {
        writeverf3 NFS3_write_verifier;
        uint64_t epoch;
    } build_verifier{};
    build_verifier.epoch = (uint64_t) nfs_ServerEpoch;

    memcpy(NFS3_write_verifier, build_verifier.NFS3_write_verifier,
           sizeof(NFS3_write_verifier));
    LOG(MODULE_NAME, D_INFO, "Get NFS3_write_verifier is %s:", NFS3_write_verifier);

    /*注册nfs服务*/
    nfs_init_svc(netconfig_udpv4, netconfig_tcpv4);
    /*注册mnt服务*/
    mnt_init_svc(netconfig_udpv4, netconfig_tcpv4);

}

/* 启动关键的处理线程 */
static void dnfs_init_threads() {
    /* 启动停止信号处理线程 */
    term_signal_handler_thread = ThreadPool::start_thread(
            "term_signal_handler", term_signal_handler);
    LOG(MODULE_NAME, D_INFO, "signal process thread start @%ld",
        term_signal_handler_thread->get_id());
}

/* dnfs启动处理函数 */
void dnfs_start() {
    if (nfs_start_info.dump_default_config) {
        dump_config();
        exit_process(0);
    }

    /* Make sure DNFS runs with a 0000 umask. */
    umask(0000);

    /* RPC Initialisation - exits on failure */
    dnfs_init_svc();

    /* 启动关键的处理线程 */
    dnfs_init_threads();

    LOG(MODULE_NAME, L_INFO,
        "-------------------------------------------------");
    LOG(MODULE_NAME, L_INFO, "             NFS SERVER INITIALIZED");
    LOG(MODULE_NAME, L_INFO,
        "-------------------------------------------------");
}
