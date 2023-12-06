/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#include <cstdarg>
#include "utils/util_exception.h"
#include "utils/common_utils.h"
using namespace std;

/*构造函数*/
UtilException::UtilException(const char *format, ...) {
    va_list args;
    va_start(args, format);
    error_message = format_message(format, args);
    va_end(args);
}

/*返回错误信息
 * return 错误信息
 * */
char const *
UtilException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT {
    return error_message.c_str();
}
