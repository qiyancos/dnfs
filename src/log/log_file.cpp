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
#include "log/log_data.h"
#include "utils/common_utils.h"

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

    /*路径判断正则表达式*/
    /*这个是非法字符匹配*/
/*
    regex path_regex_str("[<>?*\\s]+");
*/
    /*这个是合法字符匹配*/
    regex path_regex_str("(/[\\w]*)*");

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
            SET_PTR_INFO(error_info, foramt_message(
                    "The log path setting has and only three parameters such as (time,midnight,30),you set is %s",
                    args_str.c_str())
            )
            return 1;
        }
        /*解析参数*/
        /*先设置保留的日志数目*/
        if (!judge_regex(args[2], number_regex_str)) {
            SET_PTR_INFO(error_info, foramt_message(
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
                SET_PTR_INFO(error_info, foramt_message(
                        "Set the file size of the cut log setting must be a positive integer,you set is %s",
                        size_str.c_str())
                )
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
                    SET_PTR_INFO(error_info, foramt_message(
                            "Split logs by size must be less than 4gb,you set is %d GB",
                            limit_size)
                    )
                    return 1;
                }
                limit_size <<= 30;
            } else {
                SET_PTR_INFO(error_info, foramt_message(
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
                SET_PTR_INFO(error_info, foramt_message(
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
            SET_PTR_INFO(error_info, foramt_message(
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
        SET_PTR_INFO(error_info, foramt_message(
                "The log directory must be an absolute path and can only be named with numbers, letters and '_',you set is %s",
                dir_path.c_str())
        )
        return 1;
    }
    /*查看路径是否存在，不存在创建,创建错误直接返回*/
    if (creat_directory(dir_path, error_info) != 0) {
        return 1;
    }
    /*路径赋值*/
    log_directory_path = dir_path;
    return 0;
}
