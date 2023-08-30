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
#include "log/log_output.h"
#include "utils/common_utils.h"

using namespace std;

/*日志输出属性名默认构造函数*/
LogOutputAttr::LogOutputAttr() = default;

/*从配置字符串生成配置
 * 比如 "stderr:syslog:/tmp/yes@(time,midnight,30):/tmp/no"
 * 比如 "stderr:syslog:/tmp/yes@(size,10MB,30):/tmp/no"
 * params log_out_attr_str:日志文件设置
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int LogOutputAttr::generate_config(const string &log_out_attr_str,
                                   string *error_info) {
    /*设置切割保存结果*/
    vector<string> split_result;

    /*切割配置字符串*/
    split_str(log_out_attr_str, ":", split_result);

    /*清空之前的结果*/
    log_files.clear();

    for (string &param: split_result) {
        /*判断输出流开关*/
        if (param == "stderr") {
            stderr_on = true;
        } else if (param == "syslog") {
            syslog_on = true;
        } else if (param == "stdout") {
            stdout_on = true;
        } else {
            /*先建立文件属性对象*/
            LogFile log_file = LogFile();
            /*建立属性,有错误就返回*/
            if (log_file.generate_data(param, error_info) != 0) {
                return 1;
            }
            log_files.push_back(log_file);
        }
    }
    return 0;
}

/*输出日志信息
 * params module_name:模块名称
 * params message:日志信息
 * params log_level:日志等级
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int LogOutputAttr::out_message(const string &module_name,
                               const string &message,
                               const log_level_t &log_level,
                               string *error_info,...) {
    /*判断输出日志开关*/
    if (stderr_on) {
        fprintf(stderr,"%s\n",message.c_str());
    }
    if (stdout_on) {
        fprintf(stdout,"%s\n",message.c_str());
    }
    if (syslog_on) {
        /*设置空参数*/
        va_list null_list;
        va_start(null_list,error_info);
        /*打印系统日志*/
        vsyslog(log_level_info_dict[log_level].second,message.c_str(),null_list);
        va_end(null_list);
    }
    /*todo 遍历输出文件列表，进行输出*/
    for (LogFile log_file: log_files) {
        log_file.out_message(module_name, message, log_level_info_dict[log_level].first[0], error_info);
    }
    return 0;
}
