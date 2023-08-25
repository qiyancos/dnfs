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
    L_ALL,
} log_level_t;

/*不输出任何日志*/
#define LNOLOG NOLOG
/*只输出导致退出的日志*/
#define LEXIT EXIT_ERROR
/*输出普通日志以及退出日志*/
#define LRUNTIME L_INFO
/*输出DEBUG日志、普通日志和退出日志*/
#define LDEBUG D_INFO
/*输出所有日志*/
#define LEVEL_COUNT L_ALL

/*日志格式化参数*/
typedef enum LogFormatter {
    FMT_PG_NAME,
    FMT_HOST_NAME,
    FMT_LEVEL_NO,
    FMT_PATH_NAME,
    FMT_FILE_NAME,
    FMT_MD_NAME,
    FMT_FUNC_NAME,
    FMT_LINE_NO,
    FMT_WHEN_CREATED,
    FMT_RELATE_CREATED,
    FMT_ASC_TIME,
    FMT_THREAD_ID,
    FMT_THREAD_NAME,
    FMT_PROCESS_ID,
    FMT_LOG_MESSAGE,
    FMT_LOG_FORMAT_COUNT
} log_format_t;

/*存储格式字段和结构下标对应*/
static std::map<log_format_t, std::pair<std::string, log_format_t>> log_formatter_dict = {
        {FMT_PG_NAME,        {"%(program_name)",    FMT_PG_NAME}},
        {FMT_HOST_NAME,      {"%(hostname)",        FMT_HOST_NAME}},
        {FMT_LEVEL_NO,       {"%(levelno)",         FMT_LEVEL_NO}},
        {FMT_PATH_NAME,      {"%(pathname)",        FMT_PATH_NAME}},
        {FMT_FILE_NAME,      {"%(filename)",        FMT_FILE_NAME}},
        {FMT_MD_NAME,        {"%(modulename)",      FMT_MD_NAME}},
        {FMT_FUNC_NAME,      {"%(funcName)",        FMT_FUNC_NAME}},
        {FMT_LINE_NO,        {"%(lineno)",          FMT_LINE_NO}},
        {FMT_WHEN_CREATED,   {"%(created)",         FMT_WHEN_CREATED}},
        {FMT_RELATE_CREATED, {"%(relativeCreated)", FMT_RELATE_CREATED}},
        {FMT_ASC_TIME,       {"%(asctime)",         FMT_ASC_TIME}},
        {FMT_THREAD_ID,      {"%(thread)",          FMT_THREAD_ID}},
        {FMT_THREAD_NAME,    {"%(threadName)",      FMT_THREAD_NAME}},
        {FMT_PROCESS_ID,     {"%(process)",         FMT_PROCESS_ID}},
        {FMT_LOG_MESSAGE,    {"%(message)",         FMT_LOG_MESSAGE}},
};

/*初始化日志等级对照字典*/
static std::map<log_level_t, std::pair<std::vector<std::string>, int>> log_level_info_dict = {
        {L_INFO,      {{"LOG_INFO",   "LRUNTIME"}, LOG_INFO}},
        {L_WARN,      {{"LOG_WARN"},               LOG_WARNING}},
        {L_ERROR,     {{"LOG_ERROR"},              LOG_ERR}},
        {L_BACKTRACE, {{"LOG_BACKTRACE"},          LOG_ERR}},
        {D_ERROR,     {{"DEBUG_ERROR"},            LOG_ERR}},
        {D_WARN,      {{"DEBUG_WARN"},             LOG_WARNING}},
        {D_BACKTRACE, {{"DEBUG_BACKTRACE"},        LOG_ERR}},
        {D_INFO,      {{"DEBUG_INFO", "LDEBUG"},   LOG_INFO}},
        {EXIT_ERROR,  {{"EXIT_ERROR", "LEXIT"},    LOG_ALERT}},
        {NOLOG,       {{"NOLOG"},                  LOG_EMERG}},
        {L_ALL,       {{"LOG_ALL"},                LOG_INFO}},
};

#endif //DNFSD_LOG_DATA_H
