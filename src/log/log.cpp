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
#include <cstdarg>

#include "log/log.h"
#include "utils/common_utils.h"
#include "utils/thread_utils.h"
#include "log/log_message.h"

using namespace std;

Logger::Logger() = default;


/*建立日志实例*/
Logger &logger = Logger::get_instance();

/*判断日志级别
 * params log_level_str:需要判定的日志级别
 * return: 日志级别
 * */
log_level_t Logger::decode_log_level(const string &log_level_str) {

    /*遍历字典进行判定*/
    for (const auto &item: log_level_info_dict) {
        /*遍历别名列表对照*/
        for (const string &log_level: item.second.first)
            if (log_level_str == log_level) {
                return item.first;
            }
    }
    return L_ALL;
}

/*初始化全局日志类
 * params program_name_in:程序名
 * params hostname_in:运行程序所在主机名
 * */
void Logger::init(const string &program_name_in, const string &hostname_in) {
    hostname = hostname_in;
    program_name = program_name_in;
}

/*使用默认日志属性初始化一个模块日志
 * params module_name:模块名
 * return
 * */
void Logger::init_module(const string &module_name) {
    /*建立默认属性*/
    auto *init_attr = new LoggerAttr(default_attr);
    /*将属性赋值*/
    module_attr[module_name] = init_attr;
}

/*默认构造函数*/
//Logger::Logger() {
//    /*开启缓存线程*/
//    log_buffer.output_thread();
//}

/*设置退出函数,和退出状态码
 * params e_code:错误码
 * params exit_f:执行错误函数
 * */
void Logger::set_exit_func(int e_code, void (*exit_f)(int)) {
    exit_func = exit_f;
    exit_code = e_code;
}

/*对默认日志属性的设置
 * params module_name:模块名
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int
Logger::set_default_attr_from(const string &module_name, string *error_info) {
    /*如果不存在*/
    if (!judge_module_attr_exist(module_name)) {
        /*设置错误信息*/
        set_ptr_info(error_info,
                     "the module what get log attr is not exist");
        return 1;
    }
    /*查到了设置日志属性*/
    default_attr = *module_attr[module_name];
    return 0;
}

/*复制日志模板
 * params target_module_name:目标模块名
 * params src_module_name:源模块名
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::copy_module_attr_from(const string &target_module_name,
                                  const string &src_module_name,
                                  string *error_info) {
    /*如果源模块不存在*/
    if (!judge_module_attr_exist(src_module_name)) {
        /*设置错误信息*/
        set_ptr_info(error_info,
                     "the module what log attr copy from is not exist");
        return 1;
    }
    /*判断目标模块存不存在，不存在，创建在赋值,存在直接赋值*/
    if (judge_module_attr_exist(target_module_name)) {
        /*存在，直接赋值*/
        /*先复制模块的数据*/
        delete module_attr[target_module_name];
        /*复制数据*/
        auto *copy = new LoggerAttr(*module_attr[src_module_name]);
        /*重新构建数据*/
        module_attr[target_module_name] = copy;
    } else {
        set_ptr_info(error_info,
                     "the module what set log attr is not exist");
        return 1;
    }
    LogOutputAttr s = LogOutputAttr();
    return 0;
}

/*判断模块日志设置存不存在
 * params module_name:模块名
 * return: true 存在 false 不存在
 * */
bool Logger::judge_module_attr_exist(const string &module_name) {
    if (module_attr.find(module_name) != module_attr.end()) {
        return true;
    }
    return false;
}

/*得到日志单例对象
 * return: 日志对象
 * */
Logger &Logger::get_instance() {
    /*建立单例*/
    static Logger logger;
    return logger;
}

/*设置所有模块日志等级日志文件配置
 * params log_file_config:日志文件配置信息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::set_log_output(const string &log_file_config, string *error_info) {

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
    if (set_log_output(log_level_list, log_file_config, error_info) != 0) {
        return 1;
    }

    return 0;
}

/*设置所有模块单个日志等级文件配置
 * params log_level:日志级别
 * params log_file_config:日志文件配置信息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::set_log_output(const log_level_t &log_level,
                           const string &log_file_config,
                           string *error_info) {
    /*设置指定模式更改*/
    vector<log_level_t> log_level_list = {log_level};

    /*设置所有模块多个日志等级日志文件路径*/
    if (set_log_output(log_level_list, log_file_config, error_info) != 0) {
        return 1;
    }
    return 0;
}

/*设置所有模块多个日志等级日志文件配置
 * params log_level_list:需设置的日志等级列表
 * params log_file_config:日志文件配置信息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::set_log_output(const vector<log_level_t> &log_level_list,
                           const string &log_file_config,
                           string *error_info) {
    /*先构造一个日志输出对象，用来生成日志路径*/
    LogOutputAttr generate_output_attr = LogOutputAttr();
    /*解析配置,如果解析错误*/
    if (generate_output_attr.generate_config(log_file_config,
                                             error_info) != 0) {
        return 1;
    }

    /*遍历选中模式更改数据*/
    for (auto &attr: module_attr) {
        for (auto &log_level: log_level_list) {
            attr.second->log_level_output[log_level] = generate_output_attr;
        }
    }
    return 0;
}

/*设置指定模块日志等级日志文件配置
 * params module_name:模块名
 * params log_file_config:日志文件配置信息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int
Logger::set_module_log_output(const string &module_name,
                              const string &log_file_config,
                              string *error_info) {
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
    if (set_module_log_output(module_name, log_level_list, log_file_config,
                              error_info) != 0) {
        return 1;
    }
    return 0;
}

/*设置指定模块日志等级日志文件配置
 * params module_name:模块名
 * params log_level:指定的日志等级
 * params log_file_config:日志文件配置信息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::set_module_log_output(const string &module_name,
                                  const log_level_t &log_level,
                                  const string &log_file_config,
                                  string *error_info) {
    /*设置指定模式更改*/
    vector<log_level_t> log_level_list = {log_level};
    /*设置多个日志等级日志文件路径*/
    if (set_module_log_output(module_name, log_level_list, log_file_config,
                              error_info) != 0) {
        return 1;
    }
    return 0;
}

/*设置指定模块多个日志等级日志文件配置
 * params module_name:模块名
 * params log_level_list:指定的日志等级列表
 * params log_file_config:日志文件配置信息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::set_module_log_output(const string &module_name,
                                  const vector<log_level_t> &log_level_list,
                                  const string &log_file_config,
                                  string *error_info) {
    /*如果模块不存在，直接报错*/
    if (!judge_module_attr_exist(module_name)) {
        set_ptr_info(error_info, "the module what set log output is not exist");
        return 1;
    }
    /*设置选择模式的日志文件属性*/
    for (auto &log_level: log_level_list) {
        /*重新生成数据对象,错误直接返回*/
        if (module_attr[module_name]->log_level_output[log_level].generate_config(
                log_file_config, error_info) != 0) {
            return 1;
        }
    }

    return 0;
}

/*设置所有模块的日志等级，高于该等级的才可以输出
 * params log_level:指定的日志等级
 * return
 * */
void Logger::set_log_level(const log_level_t &log_level) {
    /*遍历所有属性进行设置*/
    for (auto &md: module_attr) {
        md.second->log_level = log_level;

        /*判断debug模式*/
        _judge_debug(md.second, log_level);
    }
}

/*设置指定模块日志等级，高于该等级的才可以输出
 * params module_name:模块名
 * params log_level:指定的日志等级
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::set_module_log_level(const string &module_name,
                                 const log_level_t &log_level,
                                 string *error_info) {
    /*如果模块不存在，直接报错*/
    if (!judge_module_attr_exist(module_name)) {
        set_ptr_info(error_info, "the module what set log level is not exist");
        return 1;
    }
    /*更改属性设置*/
    module_attr[module_name]->log_level = log_level;
    /*判断debug模式*/
    _judge_debug(module_attr[module_name], log_level);
    return 0;
}

/*判断debug模式
 * params log_attr:需要判定的日志属性结构体对象，直接更改其属性
 * params log_level:判定的日志等级
 * */
void Logger::_judge_debug(LoggerAttr *log_attr, log_level_t log_level) {
    /*遍历别名列表*/
    for (const string &log_l: log_level_info_dict[log_level].first)
        /*进行debug模式判定,如果包含DEBUG,设置为true*/
        if (log_l.find("DEBUG") !=
            string::npos) {
            log_attr->debug_on = true;
        } else {
            /*清空之前的设置*/
            log_attr->debug_on = false;
        }
}


/*设置所有模块日志格式
 * params format_str:格式化字符串
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int
Logger::set_formatter(const string &format_str, string *error_info) {
    /*格式化字段选择,选中为true,未选中为false*/
    vector<bool> log_formatter_select;

    /*建立日志格式*/
    if (_init_log_formatter(format_str, error_info, log_formatter_select) !=
        0) {
        return 1;
    }

    /*遍历所有模块进行创建*/
    for (auto &md_attr: module_attr) {
        /*设置格式*/
        md_attr.second->formatter = format_str;
        /*设置格式开关*/
        md_attr.second->log_formatter_select = log_formatter_select;
    }
    return 0;
}

/*设置指定模块日志格式
 * params module_name:模块名
 * params format_str:格式化字符串
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::set_module_formatter(const string &module_name,
                                 const string &format_str,
                                 string *error_info) {
    /*格式化字段选择,选中为true,未选中为false*/
    vector<bool> log_formatter_select;

    /*建立日志格式*/
    if (_init_log_formatter(format_str, error_info, log_formatter_select) !=
        0) {
        return 1;
    }

    /*如果模块不存在，直接报错*/
    if (!judge_module_attr_exist(module_name)) {
        set_ptr_info(error_info,
                     "the module what set log formatter is not exist");
        return 1;
    }

    /*设置格式*/
    module_attr[module_name]->formatter = format_str;
    /*设置格式开关*/
    module_attr[module_name]->log_formatter_select = log_formatter_select;


    return 0;
}

/*根据格式字符串，建立日志格式,供设置日志格式调用
 * params format_str:格式化字符串
 * params error_info:错误信息
 * params log_formatter_select:生成的日志格式化字段选择列表
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int
Logger::_init_log_formatter(const string &format_str, string *error_info,
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

/*设置所有模块的日期打印格式
 * params date_format:日期打印格式
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
void
Logger::set_data_format(const string &date_format) {
    /*遍历添加日志格式字符串*/
    for (const auto &md_attr: module_attr) {
        md_attr.second->date_format = date_format;
    }
}

/*设置指定模块的日期打印格式
 * params module_name:指定的模块名
 * params date_format:日期打印格式
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int
Logger::set_module_data_format(const string &module_name,
                               const string &date_format,
                               std::string *error_info) {
    /*如果模块不存在，直接报错*/
    if (!judge_module_attr_exist(module_name)) {
        set_ptr_info(error_info,
                     "the module what set date format is not exist");
        return 1;
    }

    /*设置日期格式*/
    module_attr[module_name]->date_format = date_format;
    return 0;
}


/*打印输出日志
 * params module_name:模型名
 * params log_level:日志级别
 * params file:调用文件完整路径
 * params line:调用行号
 * params func:调用方法名
 * params format:用户打印信息格式
 * params ...:用户打印信息,需对应format
 * */
void Logger::_log(const string &module_name, log_level_t log_level,
                  const string &file, const int &line, const string &func,
                  const char *format, ...) {

    /*如果模块不存在，直接报错*/
    if (!judge_module_attr_exist(module_name)) {
        throw "the module what set lof message is not exist";
    }

    /*如果满足打印日志条件*/
    if (log_level != LNOLOG and log_level != LEVEL_COUNT) {
        /*先判断打印等级满满足要求*/
        if (module_attr[module_name]->log_level >= log_level) {
            /*获取线程id*/
            thread::id tid = this_thread::get_id();

            /*创建LogMessage对象*/
            va_list args;
            va_start(args, format);

            /*构建构建对象*/
            LogMessage log_message = LogMessage(module_name,
                                                log_level,
                                                file, line,
                                                func, format, tid,
                                                module_attr[module_name], args);

            va_end(args);
            string result;
            string *error_info;
            string s;
            error_info = &s;
            if (log_message.grnarate_log_message(result, error_info) == 0) {
                cout << result << endl;
            } else {
                cout << *error_info << endl;
            }

            //    /*获取线程名*/
            //    string thread_name = ThreadPool::get_target_thread_name(tid);
            //
            //    /*将LogMessage对象加到缓存*/
            //    log_buffer.add_log_buffer(thread_name, log_message);
        }

        /*如果是退出标志*/
        if (log_level==LEXIT){
            exit_func(exit_code);
        }
    }

}

/*判断模块日志debug状态
 * return: true 开启debug false 关闭debug
 * */
bool Logger::is_module_debug_on(const string &module_name) {
    /*如果模块不存在，直接返回false*/
    if (!judge_module_attr_exist(module_name)) {
        return false;
    }
    /*返回debug状态*/
    return module_attr[module_name]->debug_on;
}

/*按照模型的日志模板格式化日志
 * params module_name:模型名
 * params log_message:根据设置的formatter生成的日志消息
 * params log_le:输出的日志级别
 * params file:调用文件完整路径
 * params line:调用行号
 * params func:调用方法名
 * params file_name:调用文件名
 * params record_time:创建时间
 * params tid:线程id
 * params pid:进程id
 * params message:用户打印的消息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int Logger::format_module_log(const string &module_name, string &log_message,
                              log_level_t log_le, const string &file,
                              const int &line, const string &func,
                              const string &file_name,
                              const time_t &record_time, const thread::id &tid,
                              const int &pid, const string &message,
                              std::string *error_info) {
    /*如果模块不存在，直接报错*/
    if (!judge_module_attr_exist(module_name)) {
        set_ptr_info(error_info,
                     "the module what format log message is not exist");
        return 1;
    }

    /*直接格式化设置日志*/
    module_attr[module_name]->get_log_message(log_message,
                                              log_le, file,
                                              line, func,
                                              file_name,
                                              record_time, tid,
                                              pid, message);

    return 0;
}

/*析构函数*/
Logger::~Logger() {
    for (const auto &attr: module_attr) {
        delete attr.second;
    }
}
