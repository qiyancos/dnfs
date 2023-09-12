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
#include <string>
#include <unistd.h>
#include <cstdarg>
#include "log/log_message.h"
#include "utils/common_utils.h"
#include "log/log.h"
#include "utils/thread_utils.h"

using namespace std;

/*初始化参数
 * params module_name:模块名
 * params log_level:日志级别
 * params file:调用文件完整路径
 * params line:调用行号
 * params func:调用方法名
 * params format:用户信息日志格式
 * params tid:线程id
 * params args:用户信息参数，需对应format
 * */
LogMessage::LogMessage(const string &module_name,
                       const LogLevel &log_level,
                       const string &file, const int &line,
                       const string &func, const char *format,
                       const thread::id &tid,
                       va_list args) {

    /*建立临时缓存存储数据*/
    char *buffer_message = (char *) malloc(ONE_MB);

    /*复制参数,超出范围重新格式化做准备*/
    va_list buffer;
    va_copy(buffer, args);

    /*格式化字符串*/
    int message_len = vsnprintf(buffer_message, ONE_MB, format, args);

    /*如果添加失败*/
    if (message_len < 0) {
        LOG("logger", L_ERROR,
            "Formatting module %s information failed", module_name.c_str());
    }

    /*判断数据大小*/
    if (message_len >= ONE_MB) {
        /*设置新的指针*/
        char *buffer_message_new;

        if (message_len < MAX_BUFFER) {
            /*动态申请内存*/
            buffer_message_new = (char *) realloc(buffer_message, message_len);
        } else {
            /*动态申请内存*/
            buffer_message_new = (char *) realloc(buffer_message, MAX_BUFFER);

            message_len = MAX_BUFFER;
            LOG("logger", L_WARN,
                "The module %s log information exceeds the size limit, the limit size is %d, the information size is %d",
                module_name.c_str(), MAX_BUFFER, message_len);
        }
        /*如果成功分配空间*/
        if (buffer_message_new) {
            /*重定向结果指针*/
            buffer_message = buffer_message_new;
            /*格式化结果*/
            message_len = vsnprintf(buffer_message, message_len, format,
                                    buffer);
            /*如果添加失败*/
            if (message_len < 0) {
                LOG("logger", L_ERROR,
                    "Module %s log message reformatting failed",
                    module_name.c_str());
            }
        } else {
            LOG("logger", L_ERROR,
                "Module %s log information failed to apply for memory",
                module_name.c_str());
        }

    }
    log_message.reset(buffer_message);

    /*参数赋值*/
    this->module_name = module_name;
    this->log_level = log_level;
    this->tid = tid;
    file_path = file;
    line_no = line;
    func_name = func;

    /*切割路径获取文件名*/
    vector<string> path_split;
    split_str(file, "/", path_split);
    file_name = path_split[path_split.size() - 1];

    /*获取进程id*/
    pid = getpid();
}

/*生成日志信息
 * params format_message:生成的日志信息
 * return
 * */
void
LogMessage::ganerate_log_message(string &format_message) {

    /*获取设置的日志格式进行判断替换*/
    format_message = logger.module_attr[module_name]->formatter + "\n";

    /*循环判定选中的格式，选中就替换*/
    for (unsigned int i = 0; i < FMT_LOG_FORMAT_COUNT; i++) {
        /*如果选择了*/
        if (logger.module_attr[module_name]->log_formatter_select[i]) {
            /*直接判定进行正则替换*/
            switch (i) {
                case 0:
                    /*项目名称*/
                    format_message = regex_replace(format_message,
                                                   regex(
                                                           "%\\(program_name\\)"),
                                                   logger.program_name);
                    break;
                case 1:
                    /*主机名*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(hostname\\)"),
                                                   logger.program_name);
                    break;
                case 2:
                    /*日志级别数字*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(levelname\\)"),
                                                   log_level_info_dict[log_level].first[0]);
                    break;
                case 3:
                    /*输出模块的完整路径名*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(pathname\\)"),
                                                   file_path);
                    break;
                case 4:
                    /*输出模块的文件名*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(filename\\)"),
                                                   file_name);
                    break;
                case 5:
                    /*模块名*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(modulename\\)"),
                                                   module_name);
                    break;
                case 6:
                    /*调用方法名*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(funcName\\)"),
                                                   func_name);
                    break;
                case 7:
                    /*调用行号*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(lineno\\)"),
                                                   to_string(line_no));
                    break;
                case 8:
                    /*当前时间，UNIX浮点数表示*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(created\\)"),
                                                   to_string(
                                                           record_time.seconds));
                    break;
                case 9:
                    /*自logger创建以来的毫秒数*/
                    format_message = regex_replace(format_message,
                                                   regex(
                                                           "%\\(relativeCreated\\)"),
                                                   to_string(
                                                           logger.get_log_init_time() -
                                                           time(nullptr)));
                    break;
                case 10:
                    /* 字符串形式的当前时间 默认为2023-08-18 11:18:45998*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(asctime\\)"),
                                                   record_time.format(
                                                           logger.module_attr[module_name]->date_format));
                    break;
                case 11:
                    /*打印线程id*/
                    /*将pid转化为字符串*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(thread\\)"),
                                                   format(tid));
                    break;
                case 12:
                    /*打印线程名*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(threadName\\)"),
                                                   ThreadPool::get_target_thread_name(
                                                           tid));
                    break;
                case 13:
                    /*打印进程id*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(process\\)"),
                                                   to_string(pid));
                    break;
                case 14:
                    /*打印用户信息*/
                    format_message = regex_replace(format_message,
                                                   regex("%\\(message\\)"),
                                                   log_message.get());
                    break;
                default:
                    break;
            }
        }

    }
}

/*判断添加调用栈
 * params format_message:生成的日志信息
 * return:
 * */
void
LogMessage::judge_traceback(string &format_message) {
    /*判断是不是需要添加调用栈,是不是debug模式，是不是含有error*/
    if (logger.module_attr[module_name]->get_debug() and
        log_level_info_dict[log_level].first[0].find("ERROR") !=
        string::npos) {
        /*保存调用栈信息*/
        string save_message;
        /*如果是debug模式，返回真正的调用栈*/
        if (string(_RUN_MODE) == "Debug") {
            save_message += get_taceback();
        } else {
            /*返回拼接的行号，方法，文件名*/
            /*获取调用栈*/
            save_message+="Traceback:\n";
            save_message+="  FILE "+file_path+",  line "+to_string(line_no)+"\n";
            save_message+="       "+func_name+"\n";
        }
        /*拼接错误信息*/
        save_message+="ErrorInfo: "+format_message;
        /*返回赋值*/
        format_message=save_message;
    }
}

/*调用输出方法
 * params message:日志信息
 * return:
 * */
void LogMessage::out_message(string &message) {
    /*调用对应的模块属性日志*/
    logger.module_attr[module_name]->out_message(log_level, message);
}