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

#include <vector>
#include "log/log_file.h"
#include "utils/common_utils.h"
#include "log/log_exception.h"
#include <cerrno>

#define FILE_MODEL_644 (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
using namespace std;

/*默认构造函数*/
LogFile::LogFile() = default;

/*解析建立数据
 * params config_str:日志文件配置信息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int LogFile::generate_data(const string &config_str, string *error_info) {
    /*先判断有没有文件设置参数
     * 获取参数切割下标*/
    size_t args_index = config_str.find('@');

    /*设置保存文件目录路径*/
    string dir_path = config_str;

    /*数字判定正则表达式*/
    regex number_regex_str("\\d+");

    /*文件目录合法字符匹配*/
    regex path_regex_str("(/[\\w]*)*");

    /*如果存在文件切割设置*/
    if (args_index != string::npos) {
        /*获取参数*/
        string args_str = config_str.substr(args_index + 2,
                                            config_str.size() - args_index - 3);
        /*保存参数*/
        vector<string> args;
        /*切割参数*/
        split_str(args_str, ",", args);
        /*判断参数数目，如果不为3直接报错*/
        if (args.size() != 3) {
            SET_PTR_INFO(error_info, format_message(
                    "The log path setting has and only three parameters such as (time,midnight,30),you set is %s",
                    args_str.c_str())
            )
            return 1;
        }
        /*解析参数*/
        /*先设置保留的日志数目*/
        if (!judge_regex(args[2], number_regex_str)) {
            SET_PTR_INFO(error_info, format_message(
                    "The size of the number of saved logs must be a positive integer,you set is %s",
                    args[2].c_str())
            )
            return 1;
        }
        /*需要保留的日志数量*/
        backup_count = stoi(args[2]);
        /*判断时间或者大小,只能存在一个*/
        if (args[0] == "size") {
            /*设置切割模式为按大小*/
            rotate_type = SIZE;
            /*不带单位报错,需要将其他单位转换为字节，根据严格规范后两位一定是单位 KB,MB,GB*/
            /*获取单位*/
            string unit_b = args[1].substr(args[1].size() - 2, args[1].size());
            /*单位转为小写*/
            to_lower(unit_b);

            /*获取数目*/
            string size_str = args[1].substr(0, args[1].size() - 2);
            /*如果是数字，转为数字*/
            if (!judge_regex(size_str, number_regex_str)) {
                SET_PTR_INFO(error_info, format_message(
                        "Set the file size of the cut log setting must be a positive integer,you set is %s",
                        size_str.c_str())
                )
                return 1;
            }
            /*转换数字*/
            log_limit.size_limit = stoll(size_str);
            /*判断单位*/
            if (unit_b == "kb") {
                log_limit.size_limit <<= 10;
            } else if (unit_b == "mb") {
                log_limit.size_limit <<= 20;
            } else if (unit_b == "gb") {
                if (log_limit.size_limit > 3) {
                    SET_PTR_INFO(error_info, format_message(
                            "Split logs by size must be less than 4gb,you set is %d GB",
                            log_limit.size_limit)
                    )
                    return 1;
                }
                log_limit.size_limit <<= 30;
            } else {
                SET_PTR_INFO(error_info, format_message(
                        "Cutting logs by size must be selected among (kb,mb,gb), and case is ignored,you set is %s",
                        unit_b.c_str())
                )
                return 1;
            }
        } else if (args[0] == "time") {
            /*设置切割模式为按时间*/
            rotate_type = TIME;
            /*日期模式转为大写*/
            to_upper(args[1]);
            if (args[1] == "NEVER") {
                /*设置不切割参数*/
                when = NEVER;
                log_limit.when_interval = 0;
            } else if (args[1] == "SECOND") {
                /*设置按秒切割*/
                when = SECOND;
                log_limit.when_interval = 1;
            } else if (args[1] == "MINUTE") {
                /*设置按分钟切割*/
                when = MINUTE;
                log_limit.when_interval = 60;
            } else if (args[1] == "HOUR") {
                /*设置按小时切割*/
                when = HOUR;
                log_limit.when_interval = 3600;
            } else if (args[1] == "DAY") {
                /*设置按天分割*/
                when = DAY;
                log_limit.when_interval = 86400;
            } else if (args[1] == "MIDNIGHT") {
                /*设置午夜切割*/
                when = MIDNIGHT;
                log_limit.when_interval = 86400;
            } else if (args[1] == "WEEK") {
                /*设置按周切割*/
                when = WEEK;
                log_limit.when_interval = 604800;
            } else {
                SET_PTR_INFO(error_info, format_message(
                        "Cutting logs by time must be selected in the list below:\n"
                        "        NEVER,\n"
                        "        SECOND,\n"
                        "        MINUTE,\n"
                        "        HOUR,\n"
                        "        DAY,\n"
                        "        MIDNIGHT,\n"
                        "        WEEK\n"
                        "and case is ignored,you set is %s", args[1].c_str())
                )
                return 1;
            }
        } else {
            SET_PTR_INFO(error_info, format_message(
                    "The log cutting limit parameter can only be selected between 'size' and 'time',you set is %s",
                    args[0].c_str())
            )
            return 1;
        }
        /*截取文件夹路径*/
        dir_path = config_str.substr(0, args_index);
    }
    /*如果路径不合法*/
    if (!judge_regex(dir_path, path_regex_str)) {
        SET_PTR_INFO(error_info, format_message(
                "The log directory must be an absolute path and can only be named with numbers, letters and '_',you set is %s",
                dir_path.c_str())
        )
        return 1;
    }
    /*查看路径是否存在，不存在创建,创建错误直接返回*/
    if (creat_directory(dir_path, error_info) != 0) {
        return 1;
    }
    /*判断是否有写权限*/
    if (access(dir_path.c_str(), W_OK) != 0) {
        SET_PTR_INFO(error_info, format_message(
                "The directory '%s' where the log file is saved does not have write permissions",
                dir_path.c_str())
        )
        return 1;
    }
    /*路径赋值*/
    log_directory_path = dir_path;
    /*生成初始日志文件路径*/
    return 0;
}

/*输出日志信息
 * params message:日志信息
 * return
 * */
void LogFile::out_message(const string &message) {
    /*生成文件句柄等写入文件操作的实例*/
    switch (rotate_type) {
        case 0:
            /*不切割日志*/
            not_rotate();
            break;
        case 1:
            /*按照时间切割*/
            rotate_by_time();
            break;
        case 2:
            /*按照大小切割*/
            rotate_by_size();
            break;
    }
    /*写文件*/
    ssize_t result = write(file_handler, message.c_str(), size(message));
    /*写入出错抛出异常*/
    if (result == -1) {
        throw LogException(
                "Failed to write information to file '%s' with module '%s' output level '%s'",
                log_file_path.c_str(), module_name.c_str(),
                log_level_info_dict[log_level].first[0].c_str());
    }
}

/*不切割日志
 * return
 * */
void
LogFile::not_rotate() {
    /*先判定日志文件名*/
    if (log_file_path.empty()) {
        /*不切割只使用模块名和日志等级*/
        log_file_path =
                log_directory_path + module_name + "_" +
                log_level_info_dict[log_level].first[0] +
                ".log";
    }
    /*判断并生成日志文件*/
    judge_and_create_log_file();
}

/*按时间切割数据方法
 * return
 * */
void LogFile::rotate_by_time() {
    /*先判定日志文件名，这里为空默认初始化*/
    if (log_file_path.empty()) {
        /*不只使用模块名，日志等级，时间标志 time*/
        log_file_path =
                log_directory_path + module_name + "_" +
                log_level_info_dict[log_level].first[0] +
                "_" + "time" + ".log";
    }
    /*判断并生成日志文件*/
    judge_and_create_log_file();
}

/*按大小切割数据方法
 * return
 * */
void LogFile::rotate_by_size() {
    /*先判定日志文件名，这里为空默认初始化*/
    if (log_file_path.empty()) {
        /*不只使用模块名，日志等级，时间标志 time*/
        log_file_path =
                log_directory_path + module_name + "_" +
                log_level_info_dict[log_level].first[0] +
                "_" + "size" + ".log";
    }
    /*判断并生成日志文件*/
    judge_and_create_log_file();

}

/*判断并生成日志文件*/
void LogFile::judge_and_create_log_file() {
    /*创建日志文件并获取句柄*/
    if (access(log_file_path.c_str(), F_OK) != 0) {
        /*尝试打开文件*/
        file_handler = open(log_file_path.c_str(), O_CREAT | O_WRONLY,
                            FILE_MODEL_644);
        /*如果文件流没有成功打开*/
        if (file_handler == -1) {
            throw LogException(
                    "The log file '%s' corresponding to the log level '%s' of the module '%s' failed to be created",
                    log_file_path.c_str(),
                    log_level_info_dict[log_level].first[0].c_str(),
                    module_name.c_str());
        }
        /*建立构造时间*/
        use_file_build_time = time(nullptr);
    }
    /*如果没有写权限*/
    if (access(log_file_path.c_str(), W_OK) != 0) {
        /*如果保存了文件流关闭*/
        if (file_handler > -1) {
            close(file_handler);
        }
        throw LogException(
                "The log file '%s' with module '%s' which log level is '%s' does not have write permission",
                log_file_path.c_str(),
                log_level_info_dict[log_level].first[0].c_str(),
                module_name.c_str());
    }
    /*文件存在，但没有获取输出文件流，获取*/
    if (file_handler == -1) {
        /*尝试打开文件,追加方式*/
        file_handler = open(log_file_path.c_str(), O_WRONLY | O_APPEND);
        /*如果文件流没有成功打开*/
        if (file_handler == -1) {
            log_file_path = "";
            throw LogException(
                    "Failed to write the log file corresponding to the log level '%s' of the module '%s'",
                    log_level_info_dict[log_level].first[0].c_str(),
                    module_name.c_str());
        }
    }
}

/*建立模块名和日志等级
 * params use_module_name:模块名
 * params out_log_level:日志输出等级
 * return
 * */
void LogFile::set_module_name_log_level(const string &use_module_name,
                                        const log_level_t &out_log_level) {
    module_name = use_module_name;
    log_level = out_log_level;
}

/*适应单独更新数据
 * params use_module_name:模块名
 * return
 * */
void LogFile::set_module_name(const string &use_module_name) {
    module_name = use_module_name;
}



