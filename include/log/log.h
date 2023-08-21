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

#include "log_file.h"

/*日志等级*/
typedef enum LogLevel {
    NOLOG,
    EXIT_ERROR,
    L_ERROR,
    L_WARN,
    L_BACKTRACE,
    L_INFO,
    D_ERROR,
    D_WARN,
    D_BACKTRACE,
    D_INFO,
    LEVEL_COUNT,
} log_level_t;

enum LogFormatter {
    PG_NAME,
    HOST_NAME,
    LEVEL_NO,
    PATH_NAME,
    FILE_NAME,
    MD_NAME,
    FUNC_NAME,
    LINE_NO,
    WHEN_CREATED,
    RELATE_CREATED,
    ASC_TIME,
    THREAD_ID,
    THREAD_NAME,
    PROCESS_ID,
    LOG_MESSAGE,
    FORMATTER_COUNT,
};

#define LOG_LEVEL_DICT_INIT { \
{L_INFO,      {"LOG_INFO",        LOG_INFO}}, \
{L_WARN,      {"LOG_WARN",        LOG_WARNING}}, \
{L_ERROR,     {"LOG_ERROR",       LOG_ERR}}, \
{L_BACKTRACE, {"LOG_BACKTRACE",   LOG_ERR}}, \
{D_ERROR,     {"DEBUG_ERROR",     LOG_ERR}}, \
{D_WARN,      {"DEBUG_WARN",      LOG_WARNING}}, \
{D_BACKTRACE, {"DEBUG_BACKTRACE", LOG_ERR}}, \
{D_INFO,      {"DEBUG_INFO",      LOG_INFO}}, \
{EXIT_ERROR,  {"EXIT_ERROR",      LOG_ALERT}}, \
{NOLOG,       {"NOLOG",           LOG_EMERG}}, \
}

/*不输出任何日志*/
#define LNOLOG NOLOG
/*只输出导致退出的日志*/
#define LEXIT EXIT_ERROR
/*输出普通日志以及退出日志*/
#define LRUNTIME L_INFO
/*输出DEBUG日志、普通日志和退出日志*/
#define LDEBUG D_INFO
/*输出所有日志*/
#define LALL LEVEL_COUNT

#define LOG(module_name, log_level, format, args...) \
    logger._log(module_name,\
            log_level,\
            __FILE__,\
            __LINE__, \
            __func__,\
            format,\
            ## args)

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
    struct LogOutputAttr {
        /*从配置字符串生成配置
         * 比如 "stderr:syslog:/tmp/a.log@(time,midnight,30):/tmp/b.log"
         * 比如 "stderr:syslog:/tmp/a.log@(size,10MB,30):/tmp/b.log"
         * */
        int generate_config(const std::string &log_out_attr_str,
                            std::string *error_info);

        /*默认的构造函数*/
        LogOutputAttr();

        /*是否输出到stderr*/
        bool stderr_on = false;
        /*是否输出到stdout*/
        bool stdout_on = false;
        /*是否输出到syslog*/
        bool syslog_on = false;
        /*是否输出到日志文件，可以同时输出到多个日志文件*/
        std::vector<LogFile> log_files;
    };

    /*一个模块日志的相关属性*/
    struct LoggerAttr {
        /*默认初始化构造函数 */
        LoggerAttr();

        /*设置程序名*/
        std::string module_name = "default";
        /*输出日志的基本格式字符串*/
        /*log formatter 打印日志格式设置
        * %(program_name) 程序名
        * %(hostname) 主机名
        * %(levelno) 数字形式的日志级别
        * %(pathname) 输出模块的完整路径名
        * %(filename) 输出模块的文件名
        * %(modulename) 输出模块名
        * %(funcName) 输出函数函数名
        * %(lineno) 输出调用日志代码所在行
        * %(created) 当前时间，UNIX浮点数表示
        * %(relativeCreated) 自logger创建以来的毫秒数
        * %(asctime) 字符串形式的当前时间 默认为2023-08-18 11:18:45998
        * %(thread) 线程id
        * %(threadName) 线程名
        * %(process) 进程id
        * %(message) 用户输出消息
        * */
        std::string formatter = "%module_name";
        /*每一个日志级别可以对应单独的输出日志文件*/
        LogOutputAttr log_level_output[LEVEL_COUNT];
        /*日志等级开关，比他小的都可以输出*/
        log_level_t log_level = NOLOG;
        /*是不是debug*/
        bool debug_on = false;

        /*格式化字段选择,选中为true,未选中为false*/
        std::vector<bool> log_formatter_select;
    };

    /*日志等级对照结构体*/
    struct LogLevelInfo {
        log_level_t level;
        std::string level_str;
        int syslog_level;
    };

private:
    /*初始化日志等级对照字典*/
    const std::map<log_level_t, std::pair<std::string, int>> log_level_info_dict = LOG_LEVEL_DICT_INIT;

    /*默认日志属性，新建日志默认使用该属性*/
    LoggerAttr default_attr;

    /*退出函数指针*/
    void (*exit_func)(int) =exit;

    /*退出函数退出码*/
    int exit_code;

    /*不同模块的日志属性*/
    std::map<std::string, LoggerAttr> module_attr;

    /*设置创建时间*/
    time_t init_time = time(nullptr);

    /*存储格式字段和结构下标对应*/
    const std::map<LogFormatter, std::pair<std::string, LogFormatter>> log_formatter_dict = {
            {PG_NAME,        {"%(program_name)",    PG_NAME}},
            {HOST_NAME,      {"%(hostname)",        HOST_NAME}},
            {LEVEL_NO,       {"%(levelno)",         LEVEL_NO}},
            {PATH_NAME,      {"%(pathname)",        PATH_NAME}},
            {FILE_NAME,      {"%(filename)",        FILE_NAME}},
            {MD_NAME,        {"%(modulename)",      MD_NAME}},
            {FUNC_NAME,      {"%(funcName)",        FUNC_NAME}},
            {LINE_NO,        {"%(lineno)",          LINE_NO}},
            {WHEN_CREATED,   {"%(created)",         WHEN_CREATED}},
            {RELATE_CREATED, {"%(relativeCreated)", RELATE_CREATED}},
            {ASC_TIME,       {"%(asctime)",         ASC_TIME}},
            {THREAD_ID,      {"%(thread)",          THREAD_ID}},
            {THREAD_NAME,    {"%(threadName)",      THREAD_NAME}},
            {PROCESS_ID,     {"%(process)",         PROCESS_ID}},
            {LOG_MESSAGE,    {"%(message)",         LOG_MESSAGE}},
    };

private:
    /*默认构造函数*/
    Logger();

public:
    /*设置主机名*/
    std::string hostname = "localhost";

    /*设置程序名*/
    std::string program_name = "Unknown";

public:
    /*初始化全局日志类*/
    void
    init(const std::string &program_name_in, const std::string &hostname_in);

    /*判断日志级别*/
    log_level_t decode_log_level(const std::string &log_level_str);

    /*对默认日志属性的设置*/
    int set_default_attr_from(const std::string &module_name,
                              std::string *error_info);

    /*设置退出函数,和退出状态码*/
    void set_exit_func(int e_code, void (*exit_f)(int));

    /*得到单例对象*/
    static Logger &get_instance();

public:

    /*使用默认日志属性初始化一个模块日志*/
    void init_module(const std::string &module_name);

    /*复制日志模板*/
    int copy_module_attr_from(const std::string &target_module_name,
                              const std::string &src_module_name,
                              std::string *error_info);

    /*判断模块日志设置存不存在*/
    bool judge_module_attr_exist(const std::string &module_name);

    /*设置所有模块日志等级日志文件属性*/
    int set_log_output(const std::string &log_path, std::string *error_info);

    /*设置指定日志等级日志文件路径*/
    int
    set_log_output(const log_level_t &log_level, const std::string &log_path,
                   std::string *error_info);

    /*设置所有模块多个日志等级日志文件路径*/
    int set_log_output(const std::vector<log_level_t> &log_level_list,
                       const std::string &log_path, std::string *error_info);

    /*设置指定模块日志等级日志文件属性*/
    int set_module_log_output(const std::string &module_name,
                              const std::string &log_path,
                              std::string *error_info);

    /*设置指定日志等级日志文件路径*/
    int set_module_log_output(const std::string &module_name,
                              const log_level_t &log_level,
                              const std::string &log_path,
                              std::string *error_info);

    /*设置多个日志等级日志文件路径*/
    int set_module_log_output(const std::string &module_name,
                              const std::vector<log_level_t> &log_level_list,
                              const std::string &log_path,
                              std::string *error_info);

    /*设置所有模块的日志等级，高于该等级的才可以输出*/
    void set_log_level(const log_level_t &log_level);

    /*设置指定模块日志等级，高于该等级的才可以输出*/
    void set_module_log_level(const std::string &module_name,
                              const log_level_t &log_level);

    /*判断debug模式*/
    void _judge_debug(LoggerAttr &log_attr,log_level_t log_level);

    /*设置所有模块日志格式*/
    int set_formatter(const std::string &format_str, std::string *error_info);

    /*设置指定模块日志格式*/
    int set_module_formatter(const std::string &module_name,
                             const std::string &format_str,
                             std::string *error_info);

    /*根据格式字符串，建立日志格式,供设置日志格式调用*/
    int _init_log_formatter(const std::string &format_str,
                             std::string *error_info,std::vector<bool> &log_formatter_select);

    /*打印输出日志*/
    void _log(const std::string &module_name, log_level_t log_level,
               const std::string &file, const int &line,
               const std::string &func, const std::string &format, ...);

    /*判断模块日志debug状态*/
    bool is_module_debug_on(const std::string &module_name);

};

/*全局唯一日志实例*/
extern Logger &logger;

#endif //LOG_LOG_H
