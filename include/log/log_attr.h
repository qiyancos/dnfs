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
#ifndef DNFSD_LOG_ATTR_H
#define DNFSD_LOG_ATTR_H

#include <string>
#include <thread>
#include "log_output.h"
#include "log_args.h"

class Logger;

class LoggerAttr {
    friend class Logger;

public:
    /*设置程序名*/
    std::string module_name = "default";
    /*输出日志的基本格式字符串*/
    /*log formatter 打印日志格式设置
    * %(program_name) 程序名
    * %(hostname) 主机名
    * %(levelname) 日志级别
    * %(pathname) 输出模块的完整路径名
    * %(filename) 输出模块的文件名
    * %(modulename) 输出模块名
    * %(funcName) 输出函数函数名
    * %(lineno) 输出调用日志代码所在行
    * %(created) 当前时间，UNIX浮点数表示
    * %(relativeCreated) 自logger创建以来的毫秒数
    * %(asctime) 字符串形式的当前时间 默认为2023-08-18 11:18:45998
    * %(thread) 线程id
    * %(threadName) 线程名
    * %(process) 进程id
    * %(message) 用户输出消息
    * */
    std::string formatter = "%(message)";

    LogOutputAttr log_level_output[LEVEL_COUNT];

    /*日志等级开关，比他小的都可以输出*/
    log_level_t log_level = L_INFO;

    /*设置时间格式
     *     %Y  Year with century as a decimal number.
     *     %m  Month as a decimal number [01,12].
     *     %d  Day of the month as a decimal number [01,31].
     *     %H  Hour (24-hour clock) as a decimal number [00,23].
     *     %M  Minute as a decimal number [00,59].
     *     %S  Second as a decimal number [00,61].
     *     %z  Time zone offset from UTC.
     *     %a  Locale's abbreviated weekday name.
     *     %A  Locale's full weekday name.
     *     %b  Locale's abbreviated month name.
     *     %B  Locale's full month name.
     *     %c  Locale's appropriate date and time representation.
     *     %I  Hour (12-hour clock) as a decimal number [01,12].
     *     %p  Locale's equivalent of either AM or PM.
     * */
    std::string date_format;

    /*是不是debug*/
    bool debug_on = false;

    /*格式化字段选择,选中为true,未选中为false*/
    bool log_formatter_select[FMT_LOG_FORMAT_COUNT]{};

public:
    /*默认初始化构造函数 */
    LoggerAttr();

    /*根据格式字符串，建立日志格式,供设置日志格式调用
     * return:
     * */
     void init_log_formatter();

    /*判断debug模式
     * return
     * */
    void judge_debug();

    /*获取debug状态
     * return 是否debug模式
     * */
    [[nodiscard]] bool get_debug() const;

    /*调用输出方法
     * params message_log_level:输出日志等级
     * params message:日志信息
     * return:
     * */
    void out_message(const log_level_t &message_log_level, const std::string &message);

    /*更改内部log_file模块名
     * params change_module_name:更改的模块名
     * */
    void set_module_name(const std::string& change_module_name);
};


#endif //DNFSD_LOG_ATTR_H
