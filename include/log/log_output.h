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
#ifndef DNFSD_LOG_OUTPUT_H
#define DNFSD_LOG_OUTPUT_H

#include <string>
#include <vector>
#include <log/log_file.h>

/*日志输出属性*/
class LogOutputAttr {
private:
    /*是否输出到stderr*/
    bool stderr_on = false;
    /*是否输出到stdout*/
    bool stdout_on = false;
    /*是否输出到syslog*/
    bool syslog_on = false;
    /*是否输出到日志文件，可以同时输出到多个日志文件*/
    std::vector<LogFile> log_files;
public:
    /*从配置字符串生成配置
     * params log_out_attr_str:日志文件设置
     * params error_info:错误信息
     * 比如 "stderr:syslog:/tmp/a.log@(time,midnight,30):/tmp/b.log"
     * 比如 "stderr:syslog:/tmp/a.log@(size,10MB,30):/tmp/b.log"
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int generate_config(const std::string &log_out_attr_str,
                        std::string *error_info);

    /*默认的构造函数*/
    LogOutputAttr();
};

#endif //DNFSD_LOG_OUTPUT_H
