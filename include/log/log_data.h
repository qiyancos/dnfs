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
#ifndef DNFSD_LOG_DATA_H
#define DNFSD_LOG_DATA_H
#include <map>
#include <syslog.h>
#include <string>
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

/*日志格式化参数*/
typedef enum LogFormatter {
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
    LOG_MESSAGE
} log_format_t;

/*存储格式字段和结构下标对应*/
extern std::map<log_format_t, std::pair<std::string, log_format_t>> log_formatter_dict;

/*初始化日志等级对照字典*/
extern std::map<log_level_t, std::pair<std::string, int>> log_level_info_dict;

#endif //DNFSD_LOG_DATA_H
