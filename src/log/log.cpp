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

/*判单日志级别是否合法，并且返回日志级别*/
log_level_t Logger::decode_log_level(const string &log_level_str) {

    /*遍历字典进行判定*/
    for (const auto &item: log_level_info_dict) {
        if (log_level_str == item.second.first) {
            return item.first;
        }
    }
    return LEVEL_COUNT;
}

/*设置全局属性，项目名称 主机名称*/
void Logger::init(const string &program_name_in, const string &hostname_in) {
    hostname = hostname_in;
    program_name = program_name_in;
}

/*使用默认日志属性初始化一个模块日志*/
void Logger::init_module(const string &module_name) {
    module_attr.insert(pair<string, LoggerAttr>(module_name, default_attr));
}

/*默认构造函数*/
Logger::Logger() = default;

/*注册退出函数,和退出状态码*/
void Logger::set_exit_func(int e_code, void (*exit_f)(int)) {
    exit_func = exit_f;
    exit_code = e_code;
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

/*复制日志模板*/
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
    return 0;
}

/*判断模块日志设置存不存在*/
bool Logger::judge_module_attr_exist(const string &module_name) {
    if (module_attr.find(module_name) != module_attr.end()) {
        return true;
    }
    return false;
}

/*得到单例对象*/
Logger &Logger::get_instance() {
    /*建立单例*/
    static Logger logger;
    return logger;
}

/*设置所有模块日志等级日志文件属性*/
int Logger::set_log_output(const string &log_path, string *error_info) {

    /*设置全模式更改*/
    vector<log_level_t> log_level_list = {EXIT_ERROR,
                                          L_ERROR,
                                          L_WARN,
                                          L_BACKTRACE,
                                          L_INFO,
                                          D_ERROR,
                                          D_WARN,
                                          D_BACKTRACE,
                                          D_INFO};
    /*设置所有模块多个日志等级日志文件路径*/
    if (set_log_output(log_level_list, log_path, error_info) != 0) {
        return 1;
    }

    return 0;
}

/*设置指定日志等级日志文件路径*/
int Logger::set_log_output(const log_level_t &log_level, const string &log_path,
                           std::string *error_info) {
    /*设置指定模式更改*/
    vector<log_level_t> log_level_list = {log_level};

    /*设置所有模块多个日志等级日志文件路径*/
    if (set_log_output(log_level_list, log_path, error_info) != 0) {
        return 1;
    }
    return 0;
}

/*设置所有模块多个日志等级日志文件路径*/
int Logger::set_log_output(const vector<log_level_t> &log_level_list,
                           const string &log_path, std::string *error_info) {
    /*先构造一个日志输出对象，用来生成日志路径*/
    LogOutputAttr generate_output_attr = LogOutputAttr();
    /*解析配置,如果解析错误*/
    if (generate_output_attr.generate_config(log_path,
                                             error_info) != 0) {
        return 1;
    }

    /*遍历选中模式更改数据*/
    for (auto &attr: module_attr) {
        for (auto &log_level: log_level_list) {
            attr.second.log_level_output[log_level] = generate_output_attr;
        }
    }
    return 0;
}

/*设置指定模块日志等级日志文件属性*/
int
Logger::set_module_log_output(const string &module_name, const string &log_path,
                              std::string *error_info) {
    /*设置全模式更改*/
    vector<log_level_t> log_level_list = {EXIT_ERROR,
                                          L_ERROR,
                                          L_WARN,
                                          L_BACKTRACE,
                                          L_INFO,
                                          D_ERROR,
                                          D_WARN,
                                          D_BACKTRACE,
                                          D_INFO};
    /*设置多个日志等级日志文件路径*/
    if (set_module_log_output(module_name, log_level_list, log_path,
                              error_info) != 0) {
        return 1;
    }
    return 0;
}

/*设置指定日志等级日志文件路径*/
int Logger::set_module_log_output(const string &module_name,
                                  const log_level_t &log_level,
                                  const string &log_path,
                                  std::string *error_info) {
    /*设置指定模式更改*/
    vector<log_level_t> log_level_list = {log_level};
    /*设置多个日志等级日志文件路径*/
    if (set_module_log_output(module_name, log_level_list, log_path,
                              error_info) != 0) {
        return 1;
    }
    return 0;
}

/*设置多个日志等级日志文件路径*/
int Logger::set_module_log_output(const string &module_name,
                                  const vector<log_level_t> &log_level_list,
                                  const string &log_path,
                                  std::string *error_info) {
    /*如果模块不存在，创建默认的模板数据*/
    if (!judge_module_attr_exist(module_name)) {
        module_attr.insert(pair<string, LoggerAttr>(module_name, default_attr));
    }
    /*设置选择模式的日志文件属性*/
    for (auto &log_level: log_level_list) {
        /*重新生成数据对象,错误直接返回*/
        if (module_attr[module_name].log_level_output[log_level].generate_config(
                log_path, error_info) != 0) {
            return 1;
        }
    }

    return 0;
}

/*设置所有模块的日志等级，高于该等级的才可以输出*/
void Logger::set_log_level(const log_level_t &log_level) {
    /*遍历所有属性进行设置*/
    for (auto &md: module_attr) {
        md.second.log_level = log_level;
    }
}

/*设置指定模块日志等级，高于该等级的才可以输出*/
void Logger::set_module_log_level(const string &module_name,
                                  const log_level_t &log_level) {
    /*如果模块不存在，创建默认的模板数据*/
    if (!judge_module_attr_exist(module_name)) {
        module_attr.insert(pair<string, LoggerAttr>(module_name, default_attr));
    }
    /*更改属性设置*/
    module_attr[module_name].log_level = log_level;

}

/*设置所有模块日志格式*/
int
Logger::set_formatter(const std::string &format_str, std::string *error_info) {
    /*格式化字段选择,选中为true,未选中为false*/
    std::vector<bool> log_formatter_select;

    /*建立日志格式*/
    if (__init_log_formatter(format_str, error_info, log_formatter_select) !=
        0) {
        return 1;
    }

    /*遍历所有模块进行创建*/
    for (auto &md_attr: module_attr) {
        /*设置格式*/
        md_attr.second.formatter = format_str;
        /*设置格式开关*/
        md_attr.second.log_formatter_select = log_formatter_select;
    }
    return 0;
}

/*设置指定模块日志格式*/
int Logger::set_module_formatter(const string &module_name,
                                 const std::string &format_str,
                                 std::string *error_info) {
    /*格式化字段选择,选中为true,未选中为false*/
    std::vector<bool> log_formatter_select;

    /*建立日志格式*/
    if (__init_log_formatter(format_str, error_info, log_formatter_select) !=
        0) {
        return 1;
    }

    /*如果模块不存在，创建默认的模板数据*/
    if (!judge_module_attr_exist(module_name)) {
        module_attr.insert(pair<string, LoggerAttr>(module_name, default_attr));
    }

    /*设置格式*/
    module_attr[module_name].formatter = format_str;
    /*设置格式开关*/
    module_attr[module_name].log_formatter_select = log_formatter_select;

    return 0;
}

/*根据格式字符串，建立日志格式,供设置日志格式调用*/
int
Logger::__init_log_formatter(const string &format_str, std::string *error_info,
                             vector<bool> &log_formatter_select) {
    /*用来判定是否设置了格式，没有设置至少一个格式报错*/
    bool set_formatter = false;
    /*循环判定是否有关键字*/
    for (auto &log_formate: log_formatter_dict) {
        /*如果查找到了关键字*/
        if (format_str.find(log_formate.second.first) != string::npos) {
            /*设置字段选中*/
            log_formatter_select.push_back(true);
            /*设置了格式*/
            set_formatter = true;
        } else {
            log_formatter_select.push_back(false);
        }
    }
    /*如果没有设置格式*/
    if (!set_formatter) {
        /*设置错误信息*/
        set_ptr_info(error_info,
                     "the formatter need select from the list:\n"
                     "        * %(program_name) the program name\n"
                     "        * %(hostname) the host name\n"
                     "        * %(levelno) the number of log level\n"
                     "        * %(pathname) the complete path for the module what use log\n"
                     "        * %(filename) the file name what for the module what use log\n"
                     "        * %(modulename) the module name\n"
                     "        * %(funcName) the function name for the module what use log\n"
                     "        * %(lineno) the line number for the module what use log\n"
                     "        * %(created) now time (UNIX float)\n"
                     "        * %(relativeCreated) the ms from log build\n"
                     "        * %(asctime) the time formatter default is 2023-08-18 11:18:45998\n"
                     "        * %(thread) the thread id\n"
                     "        * %(threadName) the thread name\n"
                     "        * %(process) the progress id\n"
                     "        * %(message) the log message");
        return 1;
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

/*模块日志属性默认构造函数*/
Logger::LoggerAttr::LoggerAttr() = default;
