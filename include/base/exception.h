/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *              Piyuyang pi_yuyang@163.com
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
#ifndef DNFSD_EXCEPTION_H
#define DNFSD_EXCEPTION_H

#include <iostream>
#include <exception>
#include <utility>

class Exception : public std::exception {
private:
    /*需要显示的错误信息*/
    std::string error_message;
public:
    /*构造函数*/
    explicit Exception(const char *format, ...);

    /*返回错误信息
     * return 错误信息
     * */
    [[nodiscard]] char const *
    what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT override;;
};


#endif //DNFSD_EXCEPTION_H
