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

#include <cstring>

#include "log/log.h"
#include "file/fsal_handle.h"
#include "utils/thread_utils.h"
#include "dnfsd/dnfsd.h"
#include "dnfsd/dnfsd_exit.h"
#include "dnfsd/dnfs_signal_proc.h"

#define MODULE_NAME "main"

/* 崩溃信号默认处理函数 */
void crash_handler(int signo, [[maybe_unused]] siginfo_t *info,
                   [[maybe_unused]] void *ctx) {
    LOG(MODULE_NAME, L_BACKTRACE, "");
    /*关闭所有的文件句柄*/
    fsal_handle.close_handles();
    /* re-raise the signal for the default signal handler to dump core */
    raise(signo);
}

/* 将处理函数挂载到对应的信号处理上 */
void install_sighandler(int signo, void (*handler)(int, siginfo_t *, void *)) {
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

/* 结束信号处理函数，用于处理停止信号对应的后续操作 */
void term_signal_handler() {
    int signal_caught = 0;

    /* Loop until we catch SIGTERM */
    while (signal_caught != SIGTERM && signal_caught != SIGINT) {
        sigset_t signals_to_catch;
        sigemptyset(&signals_to_catch);
        sigaddset(&signals_to_catch, SIGTERM);
        sigaddset(&signals_to_catch, SIGHUP);
        sigaddset(&signals_to_catch, SIGINT);
        if (sigwait(&signals_to_catch, &signal_caught) != 0) {
            LOG(MODULE_NAME, L_WARN, "sigwait exited with error");
            continue;
        }
        if (signal_caught == SIGHUP) {
            LOG(MODULE_NAME, L_INFO,
                "Received SIGHUP.... initiating export list reload");
            /* 这里需要重新加载配置文件 */
        }
    }
    LOG(MODULE_NAME, L_INFO, "term_signal_handler thread exiting");

    /* 通知所有线程关闭 */
    main_thread_pool.shutdown();

    /* 执行退出操作 */
    exit_process(0);

    return;
}
