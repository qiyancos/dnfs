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
#include "utils/time_utils.h"
#include <chrono>

using namespace std;

/*构造函数*/
Time_T::Time_T() {
    /*获取时间戳*/
    seconds = time(nullptr);
    /*获取毫秒*/
    auto now = chrono::system_clock::now();
    /*获取毫秒*/
    millseconds =
            chrono::duration_cast<chrono::milliseconds>(
                    now.time_since_epoch()).count();

    /*获取微秒*/
    microseconds = chrono::duration_cast<chrono::microseconds>(
            now.time_since_epoch()).count();
}

/*将时间戳转化为日志
 * params format:转化的日期格式
 * return: 转化完成的日期字符串
 * */
string Time_T::format(const string &format) {

    /*%Y-%m-%d %H:%M:%S*/

    /*默认的日志时间格式*/
    const char *default_time_format = "%Y-%m-%d %H:%M:%S";
    /*默认的毫秒格式*/
    const char *default_msec_format = "%s,%03d,%03d";

    /*保存转化时间*/
    char time_buffer[30] = {0};

    /*构建时间存储结构*/
    struct tm *info;

    /*将时间戳转化为为时间存储结构*/
    info = localtime(&seconds);

    /*如果设置了日期格式*/
    if (!format.empty()) {
        /*转化时间格式*/
        strftime(time_buffer, 30, format.c_str(), info);
    } else {
        /*使用默认的格式*/
        strftime(time_buffer, 30, default_time_format, info);

        /*添加毫秒数据*/
        sprintf(time_buffer, default_msec_format, time_buffer,
                (int) millseconds - seconds * 1000,
                int(microseconds) - millseconds * 1000);
    }

    return time_buffer;
}

/*重载大于运算符*/
bool Time_T::operator>(Time_T &s) const {
    if (microseconds > s.microseconds) {
        return true;
    } else {
        return false;
    }
}

/*重载小于运算符*/
bool Time_T::operator<(Time_T &s) const {
    if (microseconds < s.microseconds) {
        return true;
    } else {
        return false;
    }
}
