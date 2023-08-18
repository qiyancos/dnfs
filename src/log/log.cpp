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

#include <utility>
#include <iostream>
#include <algorithm>

#include "log/log.h"
#include "utils/common_utils.h"

using namespace std;

/*建立日志实例*/
Logger &logger = Logger::get_instance();

/*初始化日志默认属性*/
Logger::LoggerAttr Logger::default_attr;

std::map<log_level_t, std::pair<std::string, int>> Logger::log_level_info_dict = LOG_LEVEL_DICT_INIT;

/*建立默认退出指针函数*/
void (*Logger::exit_func)(int) = exit;

/*设置默认退出码*/
int Logger::exit_code = 0;

/*判单日志级别是否合法，并且返回日志级别*/
log_level_t Logger::decode_log_level(const string &log_level_str) {

    for (const auto &item: Logger::log_level_info_dict) {
        if (log_level_str == item.second.first) {
            return item.first;
        }
    }
    return LEVEL_COUNT;
}

/*设置全局属性，项目名称 主机名称*/
void Logger::init(const string &program_name, const string &hostname) {
    logger.hostname = hostname;
    logger.program_name = program_name;
}

/*使用默认日志属性初始化一个模块日志*/
void Logger::init_module(const string &module_name) {
    module_attr.insert(pair<string, LoggerAttr>(module_name, default_attr));
}

/*默认构造函数*/
Logger::Logger() = default;

/*注册退出函数,和退出状态码*/
void Logger::set_exit_func(int e_code, void (*exit_f)(int)) {
    Logger::exit_func = exit_f;
    Logger::exit_code = e_code;
}

/*对默认日志属性的设置*/
int
Logger::set_default_attr_from(const string &module_name, string *error_info) {
    /*如果不存在*/
    if (!logger.judge_module_attr_exist(module_name)) {
        /*设置错误信息*/
        set_ptr_info(error_info,
                     "do not get module log attr must set it before use");
        return 1;
    }
    /*查到了设置日志属性*/
    default_attr = logger.module_attr[module_name];
    return 0;
}

/*设置指定模块的日志属性*/
int Logger::copy_module_attr_from(const string &target_module_name,
                                  const string &src_module_name,
                                  string *error_info) {
    /*如果源模块不存在*/
    if (!judge_module_attr_exist(src_module_name)) {
        /*设置错误信息*/
        set_ptr_info(error_info,
                     "do not get src module log attr must set it before use");
        return 1;
    }
    /*判断目标模块存不存在，不存在，创建在赋值,存在直接赋值*/
    if (judge_module_attr_exist(target_module_name)) {
        /*存在，直接赋值*/
        module_attr[target_module_name] = module_attr[src_module_name];
    } else {
        /*先创建*/
        module_attr.insert(
                pair<string, LoggerAttr>(target_module_name, LoggerAttr()));
        /*在赋值*/
        module_attr[target_module_name] = module_attr[src_module_name];
    }
    LogOutputAttr s = LogOutputAttr();
    s.generate_config("stderr:syslog:/tmp/a.log@(time,MINUTE,30):/tmp/b.log",
                      error_info);
    return 0;
}

/*判断模块日志设置存不存在*/
bool Logger::judge_module_attr_exist(const string &module_name) {
    if (module_attr.find(module_name) != module_attr.end()) {
        return true;
    }
    return false;
}

Logger &Logger::get_instance() {
    /*建立单例*/
    static Logger logger;
    return logger;
}

/*设置所有模块日志等级日志文件路径*/
int Logger::set_log_output(const string &log_path, string *error_info) {

    /*先构造一个日志输出对象，用来生成日志路径*/
    LogOutputAttr generate_attr = LogOutputAttr();
    /*解析配置*/
    generate_attr.generate_config(log_path, error_info);

    /*获取遍历对象*/
    map<string, LoggerAttr>::iterator it;
    /*遍历判定更改数据*/
    for (it = module_attr.begin(); it != module_attr.end(); it++) {
        /*遍历每个日志级别的数据*/
        for (int i = 0; i < LEVEL_COUNT; ++i) {
            /*设置日志路径*/
//            it->second.log_level_output[i].log_files = vector<LogFile>
        }
    }
    return 0;
}

/*日志输出属性名默认构造函数*/
Logger::LogOutputAttr::LogOutputAttr() = default;

/*从配置字符串生成配置
 * 比如 "stderr:syslog:/tmp/a.log@(time,midnight,30):/tmp/b.log"
 * 比如 "stderr:syslog:/tmp/a.log@(size,10MB,30):/tmp/b.log"
 * */
int Logger::LogOutputAttr::generate_config(const string &log_out_attr_str,
                                           string *error_info) {
    /*设置切割保存结果*/
    vector<string> split_result;

    /*切割配置字符串*/
    split_str(log_out_attr_str, ":", split_result);

    /*遍历进行参数配置*/
    vector<string>::iterator it;
    for (it = split_result.begin(); it != split_result.end(); it++) {
        /*如果长度为空则继续*/
        if (it[0].empty()) continue;
        /*判断输出流开关*/
        if (it[0] == "stderr") {
            stderr_on = true;
        } else if (it[0] == "syslog") {
            syslog_on = true;
        } else if (it[0] == "stdout_on") {
            stdout_on = true;
        } else {
            /*先建立文件属性对象*/
            LogFile log_file = LogFile();
            /*建立属性*/
            int result_code = log_file.generate_data(it[0], error_info);
            /*有错误就返回*/
            if (result_code != 0) {
                return result_code;
            }

        }
    }
    return 0;
}

/*模块日志属性默认构造函数*/
Logger::LoggerAttr::LoggerAttr() = default;
