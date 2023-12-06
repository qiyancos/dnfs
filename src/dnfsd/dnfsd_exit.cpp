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

#include <stdlib.h>

#include "log/log.h"
#include "dnfsd/dnfsd_exit.h"

/* 该函数用于处理程序退出的时候需要执行的操作 */
void exit_process(const int exit_code) {
    /* 退出前清空所有的日志信息 */
    logger.flush();
    exit(exit_code);
}