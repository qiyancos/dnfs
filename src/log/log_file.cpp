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
#include <iostream>


#include "log/log_file.h"
#include "utils/common_utils.h"

using namespace std;

/*默认构造函数*/
LogFile::LogFile() = default;

/*解析建立数据
 * params config_str:日志文件配置信息
 * params error_info:错误信息
 * */
int LogFile::generate_data(const string &config_str, string *error_info) {
    /*先判断有没有文件设置参数
     * 获取参数切割下标*/
    size_t args_index = config_str.find('@');

    /*设置保存文件目录路径*/
    string dir_path = config_str;

    /*数字判定正则表达式*/
    regex number_regex_str("\\d+");

    /*路径判断正则表达式*/
    /*这个是非法字符匹配*/
/*
    regex path_regex_str("[<>?*\\s]+");
*/
    /*这个是合法字符匹配*/
    regex path_regex_str("^/([\\w]+/)+");

    /*如果存在*/
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
            set_ptr_info(error_info,
                         "log path config error need three args such as (time,midnight,30)");
            return 1;
        }
        /*解析参数*/
        /*先设置保留的日志数目*/
        if (!judge_regex(args[2], number_regex_str)) {
            set_ptr_info(error_info,
                         "the backup_count must be positive integer");
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
                set_ptr_info(error_info,
                             "the value of limit log file size must be positive number");
                return 1;
            }
            /*转换数字*/
            limit_size = stoll(size_str);
            /*判断单位*/
            if (unit_b == "kb") {
                limit_size <<= 10;
            } else if (unit_b == "mb") {
                limit_size <<= 20;
            } else if (unit_b == "gb") {
                if (limit_size > 3) {
                    set_ptr_info(error_info,
                                 "the value of the log file size must be smaller than 4GB");
                    return 1;
                }
                limit_size <<= 30;
            } else {
                set_ptr_info(error_info,
                             "the unit of the log file size must be in kb,mb,gb,and ignore case");
                return 1;
            }
        } else if (args[0] == "time") {
            /*设置切割模式为按时间*/
            rotate_type = TIME;
            /*日期模式转为大写*/
            to_upper(args[1]);
            if (args[1] == "NEVER") {
                when = NEVER;
            } else if (args[1] == "SECOND") {
                when = SECOND;
            } else if (args[1] == "MINUTE") {
                when = MINUTE;
            } else if (args[1] == "HOUR") {
                when = HOUR;
            } else if (args[1] == "DAY") {
                when = DAY;
            } else if (args[1] == "MIDNIGHT") {
                when = MIDNIGHT;
            } else if (args[1] == "WEEK") {
                when = WEEK;
            } else {
                set_ptr_info(error_info,
                             "the set of log file time limit must be in:"
                             "        NEVER,\n"
                             "        SECOND,\n"
                             "        MINUTE,\n"
                             "        HOUR,\n"
                             "        DAY,\n"
                             "        MIDNIGHT,\n"
                             "        WEEK\n"
                             "and ignore case");
                return 1;
            }
        } else {
            set_ptr_info(error_info,
                         "the limit of rotate log file just between size and time");
            return 1;
        }
        /*截取文件夹路径*/
        dir_path = config_str.substr(0, args_index);
    }
    /*如果路径不合法*/
    if (!judge_regex(dir_path, path_regex_str)) {
        set_ptr_info(error_info,
                     "the directory for saving log must a absolutely path what start and end with / and just contain the number alphabet _");
        return 1;
    }
    /*查看路径是否存在，不存在创建*/
    int judge_code = judge_directory_exist(dir_path);
    /*创建文件目录状态码*/
    int mk_code = 0;
    /*不存在路径直接创建*/
    if (judge_code == -1) {
        /*递归创建目录*/
        mk_code = mkdir_recursion(dir_path);
        /*命名长度超过255*/
        if (mk_code == -3) {
            set_ptr_info(error_info,
                         "the path for saving log every part length must smaller than 255");
            return 1;
        }
    }
    /*如果判断或者创建状态码为2，说明不为目录路径*/
    if (judge_code == -2 or mk_code) {
        /*不是目录路径直接返回*/
        set_ptr_info(error_info,
                     "the path for saving log is not a directory");
        return 1;
    }
    /*路径赋值*/
    log_directory_path=dir_path;
    return 0;
}
