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

#ifndef UTILS_RPC_MEM_UTIL_H
#define UTILS_RPC_MEM_UTIL_H

#include <stdlib.h>

// 该函数船体给tirpc，释放内存空间使用的函数
void rpc_free(void *p, size_t n);

// 该函数负责分配新的内存区域，并在分配失败的时候打印错误信息
void* rpc_malloc(size_t n, const char *file, int line, const char *function);

#endif //UTILS_RPC_MEM_UTIL_H
