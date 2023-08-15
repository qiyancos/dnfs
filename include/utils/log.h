/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
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

#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#include <map>
#include <string>
#include <vector>
#include <syslog.h>

/*日志等级*/
typedef enum LogLevel {
    EXIT_EXCEPTION,
    EXIT_ERROR,
    L_ERROR,
    L_WARN,
    L_INFO,
    D_ERROR,
    D_WARN,
    D_INFO,
    LEVEL_COUNT
} log_level_t;

/*不输出任何日志*/
#define LNOLOG -1
/*只输出导致退出的日志*/
#define LEXIT EXIT_ERROR
/*输出普通日志以及退出日志*/
#define LRUNTIME L_INFO
/*输出DEBUG日志、普通日志和退出日志*/
#define LDEBUG D_INFO
/*输出所有日志*/
#define LALL LEVEL_COUNT

class LogFile {
    /*日志文件的更新类型*/
    typedef enum rotate_type {
        NOT,
        TIME,
        SIZE
    } rotate_type_t;
    rotate_type_t rotate_type = NOT;

    /*如果按照时间进行日志更新，具体的日志更新时间*/
    typedef enum rotate_when {
        NEVER,
        SECOND,
        MINUTE,
        HOUR,
        DAY,
        MIDNIGHT,
        WEEK
    }rotate_when_t;
    rotate_when_t when;

    /*具体的更新时间间隔或文件大小*/
    union {
        int when_interval;
        /*按照大小更新设置限制大小，单位：字节*/
        int size_limit;
    } log_limit;

    /*需要保留的日志数量*/
    int backup_count = 0;

    /*日志文件的完整路径*/
    std::string log_file_path;

    /*TODO 其他状态信息*/
};

/*日志信息对象，存放了单词输出的日志信息结构*/
class LogMessage {

};

/*日志输出Buffer，单独线程处理，需要对多线程做多队列*/
class LogBuffer {

};

/*日志类*/
class Logger {
private:
    struct LogOutputAttr {
        /*快速初始化，使用字符串初始化
         * 比如 "stderr:syslog:/tmp/a.log@(time,midnight,30):/tmp/b.log"
         * 比如 "stderr:syslog:/tmp/a.log@(size,10MB,30):/tmp/b.log"
         * */
        LogOutputAttr(const std::string& path_str);
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
        /*设置程序名*/
        std::string module_name = "default";
        /*输出日志的基本格式字符串*/
        std::string formatter;
        /*每一个日志级别可以对应单独的输出日志文件*/
        LogOutputAttr log_level_output[LEVEL_COUNT];
        /*日志等级开关*/
        bool log_level_enable[LEVEL_COUNT];
    };

    /*日志等级对照结构体*/
    struct LogLevelInfo {
        log_level_t level;
        std::string level_str;
        int syslog_level;
    };

private:
    /*初始化日志等级对照字典*/
    const std::map<log_level_t, std::pair<std::string, int>> log_level_info_dict= {
            {L_INFO, {"LOG_INFO", LOG_NOTICE}},
    };

    /*不同模块的日志属性*/
    std::map<std::string, LoggerAttr> module_attr;

    /*默认日志属性，新建日志默认使用该属性*/
    static LoggerAttr default_attr;

public:
    /*设置主机名*/
    std::string hostname = "localhost";

    /*设置程序名*/
    std::string program_name = "Unknown";

public:
    /*初始化全局日志类*/
    static void init(const std::string program_name, const std::string& hostname);

    /*判断日志级别*/
    static log_level_t decode_log_level(const std::string& log_level_str);

public:
    /*使用默认日志属性初始化一个模块日志*/
    void init_module(const std::string& module_name);

    // 对默认日志属性的设置
    void set_default_attr(const LoggerAttr& default_attr);

    // 设置指定模块的日志属性
    void set_module_attr(const std::string& module_name, const LoggerAttr& log_attr);

    /*设置所有模块日志等级日志文件路径*/
    void set_log_output(const std::string& log_path);

    /*设置指定模块日志等级日志文件路径*/
    void set_module_log_output(const std::string& module_name, const std::string& log_path);

    /*设置所有模块多个日志等级日志文件路径*/
    void set_log_output(const std::vector<log_level_t>& log_level_list,
                        const std::string& log_path);

    /*设置多个日志等级日志文件路径*/
    void set_module_log_output(const std::string& module_name,
                               const std::vector<log_level_t>& log_level_list,
                               const std::string& log_path);

    /*设置指定日志等级日志文件路径*/
    void set_log_output(const log_level_t log_level, const std::string& log_path);

    /*设置指定日志等级日志文件路径*/
    void set_module_log_output(const log_level_t log_level, const std::string& log_path);

    /*设置所有模块的日志等级，高于该等级的才可以输出*/
    void set_log_level(const log_level_t log_level);

    /*设置指定模块日志等级，高于该等级的才可以输出*/
    void set_module_log_level(const std::string& module_name, const log_level_t log_level);

    /*设置所有模块日志格式*/
    void set_formatter(const std::string format_str);

    /*设置指定模块日志格式*/
    void set_module_formatter(const std::string& module_name, const std::string format_str);

    /*打印输出日志*/
    void log(const std::string& module_name, const log_level_t log_level,
             const std::string& format, ...);
};

/*全局唯一日志实例*/
extern Logger logger;

#endif //UTILS_LOG_H
