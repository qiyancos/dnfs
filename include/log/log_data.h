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
static std::map<log_format_t, std::pair<std::string, log_format_t>> log_formatter_dict= {
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
};;

/*初始化日志等级对照字典*/
static std::map<log_level_t, std::pair<std::vector<std::string>, int>> log_level_info_dict= {
        {L_INFO,      {{"LOG_INFO","LRUNTIME"},        LOG_INFO}},
        {L_WARN,      {{"LOG_WARN"},        LOG_WARNING}},
        {L_ERROR,     {{"LOG_ERROR"},       LOG_ERR}},
        {L_BACKTRACE, {{"LOG_BACKTRACE"},   LOG_ERR}},
        {D_ERROR,     {{"DEBUG_ERROR"},     LOG_ERR}},
        {D_WARN,      {{"DEBUG_WARN"},      LOG_WARNING}},
        {D_BACKTRACE, {{"DEBUG_BACKTRACE"}, LOG_ERR}},
        {D_INFO,      {{"DEBUG_INFO","LDEBUG"},      LOG_INFO}},
        {EXIT_ERROR,  {{"EXIT_ERROR","LEXIT"},      LOG_ALERT}},
        {NOLOG,       {{"NOLOG"},           LOG_EMERG}},
        {L_ALL,       {{"LOG_ALL"},           LOG_INFO}},
};

#endif //DNFSD_LOG_DATA_H
