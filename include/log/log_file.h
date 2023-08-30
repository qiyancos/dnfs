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

#ifndef LOG_LOG_FILE_H
#define LOG_LOG_FILE_H

#include <iostream>
#include <unistd.h>
class LogFile {
private:
    /*日志文件的更新类型*/
    typedef enum rotate_type {
        NOT,
        TIME,
        SIZE
    } rotate_type_t;

    /*如果按照时间进行日志更新，具体的日志更新时间*/
    typedef enum rotate_when {
        NEVER,
        SECOND,
        MINUTE,
        HOUR,
        DAY,
        MIDNIGHT,
        WEEK
    } rotate_when_t;

    /*具体的更新时间间隔或文件大小*/
    union {
        uint32_t when_interval;
        /*按照大小更新设置限制大小，单位：字节*/
        uint32_t size_limit;
    } log_limit{};
private:
    /*截断日志判定类型*/
    rotate_type_t rotate_type = NOT;

    /*按时间截断，时间间隔*/
    rotate_when_t when = NEVER;

    /*需要保留的日志数量*/
    int backup_count = 0;

    /*日志文件的保存目录*/
    std::string log_directory_path;

    /*记录已经生成的日志数目*/
    int log_files = 0;

    /*当前使用日志建立的时间*/
    time_t use_file_build_time;

    /*当前使用文件句柄*/
    FILE *file_handles = nullptr;

    /*当前使用日志文件名*/
    std::string log_file;

public:
    /*默认构造函数*/
    LogFile();

    /*解析建立数据
     * params config_str:日志文件配置信息
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int generate_data(const std::string &config_str, std::string *error_info);

    /*输出日志信息
     * params module_name:模块名称
     * params message:日志信息
     * params log_level_str:字符形式的日志等级
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int out_message(const std::string &module_name, const std::string &message,
                    const std::string &log_level_str, std::string *error_info);

    /*不切割日志
     * params module_name:模块名称
     * params log_level_str:字符形式的日志等级
     * */
    void not_rotate(const std::string &module_name,const std::string &log_level_str);

    /*按时间切割数据方法
     * params module_name:模块名称
     * params log_level_str:字符形式的日志等级
     * */
    void rotate_by_time(const std::string &module_name,const std::string &log_level_str);

    /*按大小切割数据方法
     * params module_name:模块名称
     * params log_level_str:字符形式的日志等级
     * */
    void rotate_by_size(const std::string &module_name,const std::string &log_level_str);

};

#endif //LOG_LOG_FILE_H
