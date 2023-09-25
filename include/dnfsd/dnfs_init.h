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

#ifndef DNFSD_DNFS_INIT_H
#define DNFSD_DNFS_INIT_H

#include <netinet/in.h>

#include <string>

#include "rpc/types.h"

/* 初始化日志相关的配置 */
void init_logging(const std::string& exec_name, const std::string& nfs_host_name,
                  log_level_t debug_level, bool detach_flag,
                  const std::string& arg_log_path);

/* 初始化错误信号的处理函数 */
void init_crash_handlers();

/* 检查malloc功能的可用性 */
void init_check_malloc();

/* 初始化线程对信号的处理操作 */
int init_thread_signal_mask();

/* dnfs启动处理函数 */
void dnfs_start();

#endif //DNFSD_DNFS_INIT_H
