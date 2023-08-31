/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#ifndef DNFSD_TIME_UTILS_H
#define DNFSD_TIME_UTILS_H

#include <ctime>
#include <cstdint>
#include <string>

class Time_T {
public:
    /*记录时间戳,用来同一个文件写入时排序*/
    time_t seconds;

    /*获取毫秒*/
    uint64_t millseconds;

    /*获取微秒*/
    uint64_t microseconds;
public:
    /*构造函数*/
    Time_T();

    /*将时间戳转化为日志
     * params format:转化的日期格式
     * return: 转化完成的日期字符串
     * */
    std::string format(const std::string &format);

    /*重载大于运算符*/
    bool operator>(Time_T &s) const;

    /*重载小于运算符*/
    bool operator<(Time_T &s) const;
};


#endif //DNFSD_TIME_UTILS_H
