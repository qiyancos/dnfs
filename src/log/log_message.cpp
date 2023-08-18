//
// Created by jiaoyue on 2023/8/17.
//

#include "log/log_message.h"

/*将时间戳转化为字符串*/
std::string LogMessage::get_record_time(const time_t &timeStamp) {

    /*保存转化时间*/
    char time_buffer[30] = {0};

    /*构建时间存储结构*/
    struct tm *info;

    /*将时间戳转化为为时间存储结构*/
    info = localtime(&timeStamp);

    /*转化时间格式*/
    strftime(time_buffer, 30, "%Y-%m-%d %H:%M:%S", info);

    return time_buffer;
}