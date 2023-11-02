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
#include <regex>
#include <dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <log/log_data.h>

#define LOG_WRITE_ERROR 0001

class LogFile {
private:
    /*具体的更新时间间隔或文件大小*/
    union {
        uint32_t when_interval;
        /*按照大小更新设置限制大小，单位：字节*/
        uint32_t size_limit;
    } log_limit{};

    /*日志文件的更新类型*/
    typedef enum rotate_type {
        NOT,
        TIME,
        SIZE
    } rotate_type_t;

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
    time_t use_file_build_time = 0;

    /*文件句柄*/
    int file_handler = -1;

    /*文件名*/
    std::string file_name;

    /*当前使用日志路径*/
    std::string log_file_path;

    /*最早切取得文件路径，用来超过限制删除*/
    std::vector<std::string> file_path_list;

    /*设置对应模块名*/
    std::string module_name = "default";

    /*设置对应输出等级*/
    log_level_t log_level = NOLOG;

    /*遇到一次的错误标志*/
    int error_flag = 0;

    /*设置路径：logfile对象字典*/
    static std::map<std::string, std::shared_ptr<LogFile>> path_log_file_dict;
    /*设置路径：属性，用来对照相同的路径，切割属性是否一样*/
    static std::map<std::string, std::string> path_rotate_attr_dict;

public:
    /*默认构造函数*/
    LogFile();

    /*解析建立数据
     * params config_str:日志文件配置信息
     * params module_n:模块名
     * params log_l:日志等级
     * return:
     * */
    void
    generate_data(const std::string &config_str, const std::string &module_n,
                  const log_level_t &log_l);

    /*输出日志信息
     * params message:日志信息
     * return
     * */
    void out_message(const std::string &message);

    /*生成日志文件路径
     * return
     * */
    void generate_log_path();

    /*不切割日志
     * return
     * */
    void not_rotate();

    /*按时间切割数据方法
     * return
     * */
    void rotate_by_time();

    /*按大小切割数据方法
     * return
     * */
    void rotate_by_size();

    /*判断并生成日志文件*/
    void judge_and_create_log_file();

    /*适应单独更新模块名
     * params use_module_name:模块名
     * return
     * */
    void set_module_name(const std::string &use_module_name);

    /*读取日志目录下的所有文件
     * return 获取的文件字符串
     * */
    std::string get_dir_file();

    /*获取切割文件编号
     * params search_file_name:需排序的文件名
     * return 获取的文件名
     * */
    std::string get_file_number(const std::string &search_file_name);

    /*获取午夜的时间戳
     * return 获取的午夜时间
     * */
    static time_t get_mid_night(const time_t &now_time);

    /*切割文件重命名
     * params now_time:记录的时间
     * return
     * */
    void rotate_log_file(const time_t &now_time);

    /*判定生成日志文件对象
     * params config_str:日志文件配置信息
     * params module_n:模块名
     * params log_l:日志等级
     * return: 获取的日志对象
     * */
    static std::shared_ptr<LogFile>
    get_log_file(const std::string &config_str, const std::string &module_n,
                 const log_level_t &log_l);

    /*获取日志路径
     * return: 日志路径
     * */
    std::string get_log_path();

    /*析构函数*/
    ~LogFile();
};

#endif //LOG_LOG_FILE_H
