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
#include "log/log_exception.h"
#include "utils/common_utils.h"

using namespace std;

/*日志输出属性名默认构造函数*/
LogOutputAttr::LogOutputAttr() = default;

/*从配置字符串生成配置
 * 比如 "stderr:syslog:/tmp/yes@(time,midnight,30):/tmp/no"
 * 比如 "stderr:syslog:/tmp/yes@(size,10MB,30):/tmp/no"
 * params log_out_attr_str:日志文件设置
 * return
 * */
void LogOutputAttr::generate_config(const string &log_out_attr_str) {
    /*设置切割保存结果*/
    vector<string> split_result;

    /*切割配置字符串*/
    split_str(log_out_attr_str, ":", split_result);

    /*清空之前的结果*/
    log_files.clear();

    for (const string &param: split_result) {
        /*判断输出流开关*/
        if (param == "stderr") {
            stderr_on = true;
        } else if (param == "syslog") {
            syslog_on = true;
        } else if (param == "stdout") {
            stdout_on = true;
        } else {
            /*获取智能指针*/
//            shared_ptr<LogFile> log_file_ptr;
//            log_file_ptr.reset(&log_file);
            /*添加指针*/
            log_files.push_back(
                    LogFile::get_log_file(param, module_name, log_level));
        }
    }
}

/*建立模块名和日志等级
 * params module_n:模块名
 * params out_log_level:日志输出等级
 * return
 * */
void LogOutputAttr::set_module_name_log_level(const string &module_n,
                                              const log_level_t &out_log_level) {
    /*保存日志等级*/
    log_level = out_log_level;
    /*设置模块名*/
    module_name = module_n;
}

/*适应单独更新模块名
 * params module_n:模块名
 * return
 * */
void LogOutputAttr::set_module_name(const string &module_n) {
    /*设置模块名*/
    module_name = module_n;
    /*遍历建立模块信息*/
    for (auto &log_file: log_files) {
        log_file->set_module_name(module_n);
    }
}

/*输出日志信息
 * params module_name:模块名称
 * params message:日志信息
 * params log_level:日志等级
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int LogOutputAttr::out_message(const string &message,
                               string *error_info, ...) {
    /*判断输出日志开关*/
    if (stderr_on) {
        fprintf(stderr, "%s", message.c_str());
    }
    if (stdout_on) {
        fprintf(stdout, "%s", message.c_str());
    }
    if (syslog_on) {
        /*设置空参数*/
        va_list null_list;
        va_start(null_list, error_info);
        /*打印系统日志*/
        vsyslog(log_level_info_dict[log_level].second, message.c_str(),
                null_list);
        va_end(null_list);
    }
    /*捕获写日志文件的异常,除了问题将信息写入系统日志*/
    for (auto &log_file: log_files) {
        try {
            /*写日志文件*/
            log_file->out_message(message);
        } catch (LogException &e) {
            /*将日志信息写入系统日志*/
            if (!syslog_on) {
                /*设置空参数*/
                va_list null_list;
                va_start(null_list, error_info);
                /*打印系统日志*/
                vsyslog(log_level_info_dict[log_level].second, message.c_str(),
                        null_list);
                va_end(null_list);
            }
            /*写文件出错*/
            SET_PTR_INFO(error_info, e.what())
            return 1;
        }
    }
    return 0;
}
