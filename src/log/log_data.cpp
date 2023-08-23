//
// Created by jiaoyue on 2023/8/23.
//
using namespace std;
#include "log/log_data.h"
map<log_format_t, std::pair<std::string, log_format_t>>log_formatter_dict= {
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

map<log_level_t, std::pair<std::string, int>> log_level_info_dict= {
{L_INFO,      {"LOG_INFO",        LOG_INFO}},
{L_WARN,      {"LOG_WARN",        LOG_WARNING}},
{L_ERROR,     {"LOG_ERROR",       LOG_ERR}},
{L_BACKTRACE, {"LOG_BACKTRACE",   LOG_ERR}},
{D_ERROR,     {"DEBUG_ERROR",     LOG_ERR}},
{D_WARN,      {"DEBUG_WARN",      LOG_WARNING}},
{D_BACKTRACE, {"DEBUG_BACKTRACE", LOG_ERR}},
{D_INFO,      {"DEBUG_INFO",      LOG_INFO}},
{EXIT_ERROR,  {"EXIT_ERROR",      LOG_ALERT}},
{NOLOG,       {"NOLOG",           LOG_EMERG}},
};