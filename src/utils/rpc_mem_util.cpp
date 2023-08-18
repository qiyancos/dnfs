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

#include <exception>

#include "log/log.h"
#include "utils/rpc_mem_utils.h"

#define MODULE_NAME "rpc_mem"

using namespace std;

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
