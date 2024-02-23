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
#ifndef DNFSD_LOG_OUTPUT_H
#define DNFSD_LOG_OUTPUT_H

#include <string>
#include <vector>
#include <syslog.h>
#include <cstdarg>
#include <log/log_file.h>
#include <log/log_args.h>

/*日志输出属性*/
class LogOutputAttr {
private:
    /*输出日志等级*/
    log_level_t log_level = NOLOG;
    /*模块名*/
    std::string module_name = "default";
    /*是否输出到stderr*/
    bool stderr_on = false;
    /*是否输出到stdout*/
    bool stdout_on = false;
    /*是否输出到syslog*/
    bool syslog_on = false;
    /*是否输出到日志文件，可以同时输出到多个日志文件*/
    std::vector<std::shared_ptr<LogFile>> log_files;
public:
    /*从配置字符串生成配置
     * 比如 "stderr:syslog:/tmp/yes@(time,midnight,30):/tmp/no"
     * 比如 "stderr:syslog:/tmp/yes@(size,10MB,30):/tmp/no"
     * params log_out_attr_str:日志文件设置
     * return
     * */
    void generate_config(const std::string &log_out_attr_str);

    /*默认的构造函数*/
    LogOutputAttr();

    /*建立模块名和日志等级
     * params module_n:模块名
     * params out_log_level:日志输出等级
     * return
     * */
    void set_module_name_log_level(const std::string &module_n,
                                   const log_level_t &out_log_level);

    /*适应单独更新模块名
     * params module_n:模块名
     * return
     * */
    void set_module_name(const std::string &module_n);

    /*输出日志信息
     * params message:日志信息
     * return:
     * */
    void out_message(const std::string &message);

};

#endif //DNFSD_LOG_OUTPUT_H
