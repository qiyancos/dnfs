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
 * params log_out_attr_str:日志文件设置
 * params error_info:错误信息
 * 比如 "stderr:syslog:/tmp/a.log@(time,midnight,30):/tmp/b.log"
 * 比如 "stderr:syslog:/tmp/a.log@(size,10MB,30):/tmp/b.log"
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
        } else if (param == "stdout_on") {
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