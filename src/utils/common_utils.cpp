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
#include <sys/stat.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdarg>
#include <algorithm>
#include <iostream>
#include <experimental/filesystem>

#include "utils/common_utils.h"

using namespace std;

bool _beauty = false;
int _indent = 4;
string _indent_str = "    ";
map<const void *, int> print_depth;

/* 将二进制的socket地址转化为可读的字符串 */
const std::string format(const sockaddr_storage &out_data) {
    int port = 0;
    const char *name = NULL;
    char ipname[SOCK_NAME_MAX];

    switch (out_data.ss_family) {
        case AF_INET:
            name = inet_ntop(out_data.ss_family,
                             &(((struct sockaddr_in *) &out_data)->sin_addr),
                             ipname,
                             sizeof(ipname));
            port = ntohs(((struct sockaddr_in *) &out_data)->sin_port);
            break;

        case AF_INET6:
            name = inet_ntop(out_data.ss_family,
                             &(((struct sockaddr_in6 *) &out_data)->sin6_addr),
                             ipname,
                             sizeof(ipname));
            port = ntohs(((struct sockaddr_in6 *) &out_data)->sin6_port);
            break;

        case AF_LOCAL:
            return ((struct sockaddr_un *) &out_data)->sun_path;
    }

    if (name == NULL) {
        return "<unknown>";
    } else {
        return string(name) + to_string(port);
    }
}

/* 该函数用于设置format的格式，设置为True则会追加缩进和换行 */
void set_print_beauty(bool beauty) {
    _beauty = beauty;
}

/* 设置美化输出情况下自动所进的空格数量，默认为4 */
[[maybe_unused]] void set_print_indent(int indent) {
    _indent = indent;
    _indent_str = string(indent, ' ');
}

/*切割字符串*/
void split_str(const string &str, const string &split, vector<string> &result) {
    /*如果字符串为空，直接返回*/
    if (str.empty()) return;

    /*在字符串末尾也加入分隔符，方便截取最后一段*/
    string strs = str + split;

    /*先查找第一个符号位置*/
    size_t pos = strs.find(split);

    /*获取分割字符长度，用来截取剩余字符串*/
    size_t step = split.size();

    /*若找不到内容则字符串搜索函数返回 npos*/
    while (pos != string::npos) {
        /*截取获取的字符串*/
        string temp = strs.substr(0, pos);

        /*结果保存*/
        if (!temp.empty()) {
            result.push_back(temp);
        }

        /*去掉已分割的字符串,在剩下的字符串中进行分割*/
        strs = strs.substr(pos + step, strs.size());

        /*接着查询分隔符进行切割*/
        pos = strs.find(split);
    }
}

/*验证字符串是否满足正则表达式
 * params judge_str:验证的字符串
 * params judge_str:验证的正则表达式
 * return: true 匹配成功 false 匹配失败
 * */
bool judge_regex(const string &judge_str, const regex &regex_expression) {
    /*返回匹配结果*/
    return regex_match(judge_str, regex_expression);
}

/*转为小写
 * params str:需转换的字符串
 * return
 * */
void to_lower(string &str) {
    /*全转为小写*/
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}


/*转为大写
 * params str:需转换的字符串
 * return
 * */
void to_upper(string &str) {
    /*全转为小写*/
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}

/*判断文件目录是否存在
 * params judge_dir:验证的路径
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int creat_directory(const string &judge_dir, string *error_info) {
    struct stat info{};
    if (stat(judge_dir.c_str(), &info) != 0) {
        try {
            /*递归创建文件夹*/
            experimental::filesystem::create_directories(judge_dir);
            /*创建成功*/
            return 0;
        } catch (experimental::filesystem::filesystem_error &e) {
            /*添加错误信息*/
            SET_PTR_INFO(error_info, e.what())
            /*创建错误返回失败*/
            return 1;
        }
    } else if (info.st_mode & S_IFDIR) {
        /*目录存在*/
        return 0;
    } else {
        /*设置错误信息*/
        SET_PTR_INFO(error_info, foramt_message(
                "The storage log path '%s' set is not a directory",
                judge_dir.c_str()))
        /*不是文件目录返回错误*/
        return 1;
    }
}

/*将时间戳转化为日志
 * params timeStamp:转化的时间戳
 * params format:转化的日期格式
 * return: 转化完成的日期字符串
 * */
string get_record_time(const time_t &timeStamp, const string &format) {

    /*%Y-%m-%d %H:%M:%S*/

    /*默认的日志时间格式*/
    const char *default_time_format = "%Y-%m-%d %H:%M:%S";
    /*默认的毫秒格式*/
    const char *default_msec_format = "%s,%03d";

    /*保存转化时间*/
    char time_buffer[30] = {0};

    /*构建时间存储结构*/
    struct tm *info;

    /*将时间戳转化为为时间存储结构*/
    info = localtime(&timeStamp);

    /*如果设置了日期格式*/
    if (!format.empty()) {
        /*转化时间格式*/
        strftime(time_buffer, 30, format.c_str(), info);
    } else {
        /*使用默认的格式*/
        strftime(time_buffer, 30, default_time_format, info);

        /*获取系统当前时间*/
        auto now = chrono::system_clock::now();

        /*通过不同精度获取相差的毫秒数*/
        uint64_t dis_millseconds = chrono::duration_cast<chrono::milliseconds>(
                now.time_since_epoch()).count()
                                   - chrono::duration_cast<chrono::seconds>(
                now.time_since_epoch()).count() * 1000;
        /*添加毫秒数据*/
        sprintf(time_buffer, default_msec_format, time_buffer,
                (int) dis_millseconds);
    }

    return time_buffer;
}

/*将pid转为字符串
 * params t:任意类型字符串
 * return: 转化完成的字符串
 * */
string pid_to_string(const thread::id &t) {
    /*建立操作实例*/
    std::stringstream s_stream;
    /*将数据输入操作实例*/
    s_stream << t;
    /*返回结果*/
    return s_stream.str();
}

/*按照格式格式化字符串
 * params format:格式化字符串
 * params ... :参数
 * return:格式完成的数据
 * */
string foramt_message(const char *foramt, ...) {
    char buffer[ONE_MB];
    va_list va;
    va_start(va, foramt);
    /*格式化字符串*/
    vsnprintf(buffer, 1024, foramt, va);
    va_end(va);
    return buffer;
}