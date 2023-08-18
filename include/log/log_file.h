//
// Created by jiaoyue on 2023/8/17.
//
#ifndef LOG_USE_LOG_FILE_H
#define LOG_USE_LOG_FILE_H
#include <iostream>

class LogFile {
private:
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
    } rotate_when_t;
    rotate_when_t when=NEVER;

    /*具体的更新时间间隔或文件大小*/
    union {
        int when_interval;
        /*按照大小更新设置限制大小，单位：字节*/
        int size_limit;
    } log_limit{};

    /*需要保留的日志数量*/
    int backup_count = 0;

    /*设置文件限制大小*/
    uint32_t limit_size=102400;

    /*日志文件的完整路径*/
    std::string log_file_path;

    /*TODO 其他状态信息*/
public:
    /*默认构造函数*/
    LogFile();
    /*解析建立数据*/
    int generate_data(const std::string &config_str,std::string *error_info);
};



#endif //LOG_USE_LOG_FILE_H
