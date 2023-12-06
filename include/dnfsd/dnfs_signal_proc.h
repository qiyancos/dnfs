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

#ifndef DNFSD_DNFS_SIGNAL_PROC_H
#define DNFSD_DNFS_SIGNAL_PROC_H

#include <signal.h>

/* 崩溃信号默认处理函数 */
void crash_handler(int signo, [[maybe_unused]] siginfo_t *info,
                   [[maybe_unused]] void *ctx);

/* 将处理函数挂载到对应的信号处理上 */
void install_sighandler(int signo, void (*handler)(int, siginfo_t *, void *));

/* 结束信号处理函数，用于处理停止信号对应的后续操作 */
void term_signal_handler();

#endif //DNFSD_DNFS_SIGNAL_PROC_H
