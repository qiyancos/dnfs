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

#include "log/log_message.h"
#include "utils/common_utils.h"
#include <string>

using namespace std;

/*初始化参数
 * params module_name:模块名
 * params log_level:日志级别
 * params file:调用文件完整路径
 * params line:调用行号
 * params func:调用方法名
 * params format:用户信息日志格式
 * params args:用户信息参数，需对应format
 * */
LogMessage::LogMessage(const std::string &module_name,
                       const LogLevel &log_level,
                       const std::string &file, const int &line,
                       const std::string &func, const char *format,
                       va_list args) {

    /*建立临时缓存存储数据*/
    char temporary[100];

    /*格式化字符串*/
    int message_len = vsnprintf(temporary, 100, format, args);

    /*判断数据大小*/
    if(message_len>100){
        if(100<=message_len<MAX_BUFFER){
            /*动态申请内存*/
            log_message= (char *)malloc(message_len);
        }else{
            /*动态申请内存*/
            log_message= (char *)malloc(MAX_BUFFER);
            /*打印日志超出警告日志*/
        }
        int result=vsnprintf(log_message, 100, format, args);
        /*如果添加失败*/
        if(result<0){
           /*todo 打印日志错误日志*/
        }
    }else{
        log_message=temporary;
    }


    /*参数赋值*/
    this->module_name = module_name;
    this->log_level = log_level;
    file_path = file;
    line_no = line;
    func_name = func;
    record_time = time(nullptr);

    /*切割路径获取文件名*/
    vector<string> path_split;
    split_str(file, "/", path_split);
    file_name = path_split[path_split.size() - 1];
}

/*析构函数*/
LogMessage::~LogMessage() {
    /*释放指针*/
    if (log_message) {
        free(log_message);
    }
}
