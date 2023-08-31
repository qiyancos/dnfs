/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
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

#ifndef LOG_LOG_H
#define LOG_LOG_H

#include <map>
#include <string>
#include <vector>
#include <syslog.h>
#include <atomic>
#include <queue>
#include <thread>
#include <set>

#include "log_data.h"
#include "log_attr.h"
#include "log_buffer.h"

/*打印日志*/
#define LOG(module_name, log_level, format, args...) \
    logger._log(module_name,\
                log_level,\
                __FILE__,\
                __LINE__, \
                __func__,\
                format,\
                ## args)

/*判断打印日志*/
#define LOG_IF(log_flag, module_name, log_level, format, args...) \
    if(log_flag) { \
        logger._log(module_name,\
                    log_level,\
                    __FILE__,\
                    __LINE__, \
                    __func__,\
                    format,\
                    ## args); \
    }

/*日志类*/
class Logger {

private:

    /*默认日志属性，新建日志默认使用该属性*/
    LoggerAttr default_attr;

    /*退出函数指针*/
    void (*exit_func)(int) =exit;

    /*退出函数退出码*/
    int exit_code = 0;

    /*不同模块的日志属性*/
    std::map<std::string, LoggerAttr *> module_attr;

    /*设置创建时间*/
    time_t init_time = time(nullptr);

    /*缓存日志对象*/
    LogBuffer log_buffer = LogBuffer();

    /*设置全模式更改*/
    std::vector<log_level_t> all_log_level = {EXIT_ERROR,
                                              L_ERROR,
                                              L_WARN,
                                              L_BACKTRACE,
                                              L_INFO,
                                              D_ERROR,
                                              D_WARN,
                                              D_BACKTRACE,
                                              D_INFO};
private:
    /*默认构造函数*/
    Logger();

public:

    /*设置主机名*/
    std::string hostname = "localhost";

    /*设置程序名*/
    std::string program_name = "Unknown";

public:
    /*得到日志单例对象
     * return: 日志对象
     * */
    static Logger &get_instance();

public:
    /*初始化全局日志类
     * params program_name_in:程序名
     * params hostname_in:运行程序所在主机名
     * */
    void
    init(const std::string &program_name_in, const std::string &hostname_in);

    /*判断日志级别
     * params log_level_str:需要判定的日志级别
     * return: 日志级别
     * */
    static log_level_t decode_log_level(const std::string &log_level_str);

    /*对默认日志属性的设置
     * params module_name:模块名
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_default_attr_from(const std::string &module_name,
                              std::string *error_info);

    /*设置退出函数,和退出状态码
     * params e_code:错误码
     * params exit_f:执行错误函数
     * return
     * */
    void set_exit_func(int e_code, void (*exit_f)(int));

    /*使用默认日志属性初始化一个模块日志
     * params module_name:模块名
     * return
     * */
    void init_module(const std::string &module_name);

    /*复制日志模板
     * params target_module_name:目标模块名
     * params src_module_name:源模块名
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int copy_module_attr_from(const std::string &target_module_name,
                              const std::string &src_module_name,
                              std::string *error_info);

    /*判断模块日志设置存不存在
     * params module_name:模块名
     * return: true 存在 false 不存在
     * */
    bool judge_module_attr_exist(const std::string &module_name);

    /*设置所有模块日志等级日志文件配置
     * params log_file_config:日志文件配置信息
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int
    set_log_output(const std::string &log_file_config, std::string *error_info);

    /*设置所有模块单个日志等级文件配置
     * params log_level:日志级别
     * params log_file_config:日志文件配置信息
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int
    set_log_output(const log_level_t &log_level,
                   const std::string &log_file_config,
                   std::string *error_info);

    /*设置所有模块多个日志等级日志文件配置
     * params log_level_list:需设置的日志等级列表
     * params log_file_config:日志文件配置信息
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_log_output(const std::vector<log_level_t> &log_level_list,
                       const std::string &log_file_config,
                       std::string *error_info);

    /*设置指定模块日志等级日志文件配置
     * params module_name:模块名
     * params log_file_config:日志文件配置信息
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_module_log_output(const std::string &module_name,
                              const std::string &log_file_config,
                              std::string *error_info);

    /*设置指定模块日志等级日志文件配置
     * params module_name:模块名
     * params log_level:指定的日志等级
     * params log_file_config:日志文件配置信息
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_module_log_output(const std::string &module_name,
                              const log_level_t &log_level,
                              const std::string &log_file_config,
                              std::string *error_info);

    /*设置指定模块多个日志等级日志文件配置
     * params module_name:模块名
     * params log_level_list:指定的日志等级列表
     * params log_file_config:日志文件配置信息
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_module_log_output(const std::string &module_name,
                              const std::vector<log_level_t> &log_level_list,
                              const std::string &log_file_config,
                              std::string *error_info);

    /*设置所有模块的日志等级，高于该等级的才可以输出
     * params log_level:指定的日志等级
     * return
     * */
    void set_log_level(const log_level_t &log_level);

    /*设置指定模块日志等级，高于该等级的才可以输出
     * params module_name:模块名
     * params log_level:指定的日志等级
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_module_log_level(const std::string &module_name,
                             const log_level_t &log_level,
                             std::string *error_info);

    /*设置所有模块日志格式
     * params format_str:格式化字符串
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_formatter(const std::string &format_str, std::string *error_info);

    /*设置指定模块日志格式
     * params module_name:模块名
     * params format_str:格式化字符串
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_module_formatter(const std::string &module_name,
                             const std::string &format_str,
                             std::string *error_info);

    /*设置所有模块的日期打印格式
     * params date_format:日期打印格式
     * return
     * */
    void set_date_format(const std::string &date_format);

    /*设置指定模块的日期打印格式
     * params module_name:指定的模块名
     * params date_format:日期打印格式
     * params error_info:错误信息
     * return: 状态码 0 生成成功 其他 生成失败
     * */
    int set_module_date_format(const std::string &module_name,
                               const std::string &date_format,
                               std::string *error_info);

    /*打印输出日志
     * params module_name:模型名
     * params log_level:日志级别
     * params file:调用文件完整路径
     * params line:调用行号
     * params func:调用方法名
     * params format:用户打印信息格式
     * params ...:用户打印信息,需对应format
     * return
     * */
    void _log(const std::string &module_name, log_level_t log_level,
              const std::string &file, const int &line,
              const std::string &func, const char *format, ...);

    /*判断模块日志debug状态
     * params module_name:模型名
     * return: true 开启debug false 关闭debug
     * */
    bool is_module_debug_on(const std::string &module_name);

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
    int format_module_log(const std::string &module_name,
                          std::string &log_message,
                          log_level_t log_le,
                          const std::string &file,
                          const int &line, const std::string &func,
                          const std::string &file_name,
                          const time_t &record_time,
                          const std::thread::id &tid,
                          const int &pid,
                          const std::string &message,
                          std::string *error_info);

    /*将所有的模板设置为默认属性
     * return
     * */
    void set_all_module_attr_default();

    /*获取log模块建立时间*/
    [[nodiscard]] time_t get_log_init_time() const;

    /*设置缓存大小
     * params buffer_limit:设置的缓存大小
     * return
     * */
    void set_buffer_limit(const int &buffer_limit);

    /*清空缓存*/
    void flush();

    ~Logger();

};

/*全局唯一日志实例*/
extern Logger &logger;

#endif //LOG_LOG_H
