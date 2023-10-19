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

#define BOOST_STACKTRACE_USE_ADDR2LINE

#include <boost/stacktrace.hpp>
#include <netinet/in.h>
#include <cstdarg>
#include <algorithm>
#include <experimental/filesystem>

#include "utils/common_utils.h"
#include "utils/unit_exception.h"

using namespace std;

bool _beauty = false;
int _indent = 4;
string _indent_str = "    ";
map<const void *, int> print_depth;

/* 将二进制的socket地址转化为可读的字符串 */
std::string format(sockaddr_storage *out_data) {
    int port = 0;
    const char *name = nullptr;
    char ipname[SOCK_NAME_MAX];

    switch (out_data->ss_family) {
        case AF_INET:
            name = inet_ntop(out_data->ss_family,
                             &(((struct sockaddr_in *) out_data)->sin_addr),
                             ipname,
                             sizeof(ipname));
            port = ntohs(((struct sockaddr_in *) out_data)->sin_port);
            break;

        case AF_INET6:
            name = inet_ntop(out_data->ss_family,
                             &(((struct sockaddr_in6 *) out_data)->sin6_addr),
                             ipname,
                             sizeof(ipname));
            port = ntohs(((struct sockaddr_in6 *) out_data)->sin6_port);
            break;

        case AF_LOCAL:
            return ((struct sockaddr_un *) out_data)->sun_path;
    }

    if (name == nullptr) {
        return "<unknown>";
    } else {
        return string(name) + ":" + to_string(port);
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
 * return
 * */
void creat_directory(const string &judge_dir) {
    struct stat info{};
    if (stat(judge_dir.c_str(), &info) != 0) {
        /*递归创建文件夹,有异常直接报*/
        experimental::filesystem::create_directories(judge_dir);
    } else if (!(info.st_mode & S_IFDIR)) {
        /*如果存在但不是文件夹*/
        throw UnitException(
                "The storage log path '%s' set is not a directory",
                judge_dir.c_str());
    }
}

/*将时间戳转化为日志
 * %a 星期几的简写
   %A 星期几的全称
   %b 月分的简写
   %B 月份的全称
   %c 标准的日期的时间串
   %C 年份的后两位数字
   %d 十进制表示的每月的第几天
   %D 月/天/年
   %e 在两字符域中，十进制表示的每月的第几天
   %F 年-月-日
   %g 年份的后两位数字，使用基于周的年
   %G 年分，使用基于周的年
   %h 简写的月份名
   %H 24小时制的小时
   %I 12小时制的小时
   %j 十进制表示的每年的第几天
   %m 十进制表示的月份
   %M 十时制表示的分钟数
   %n 新行符
   %p 本地的AM或PM的等价显示
   %r 12小时的时间
   %R 显示小时和分钟：hh:mm
   %S 十进制的秒数
   %t 水平制表符
   %T 显示时分秒：hh:mm:ss
   %u 每周的第几天，星期一为第一天 （值从0到6，星期一为0）
   %U 第年的第几周，把星期日做为第一天（值从0到53）
   %V 每年的第几周，使用基于周的年
   %w 十进制表示的星期几（值从0到6，星期天为0）
   %W 每年的第几周，把星期一做为第一天（值从0到53）
   %x 标准的日期串
   %X 标准的时间串
   %y 不带世纪的十进制年份（值从0到99）
   %Y 带世纪部分的十进制年份
   %z，%Z 时区名称，如果不能得到时区名称则返回空字符。
   %% 百分号
 * params time_stamp:转化的时间戳
 * params dis_millseconds:转化的毫秒
 * params format:转化的日期格式
 * return: 转化完成的日期字符串
 * */
string format(const time_t &time_stamp, const uint64_t &dis_millseconds,
              const string &format) {

    /*%Y-%m-%d %H:%M:%S*/

    /*默认的日志时间格式*/
    const char *default_time_format = "%Y-%m-%d %H:%M:%S";
    /*默认的毫秒格式*/
    const char *default_msec_format = "%s,%03d";

    /*保存转化时间*/
    char time_buffer[30];

    /*构建时间存储结构*/
    struct tm *info;

    /*将时间戳转化为为时间存储结构*/
    info = localtime(&time_stamp);

    /*如果设置了日期格式*/
    if (!format.empty()) {
        /*转化时间格式*/
        strftime(time_buffer, 30, format.c_str(), info);
    } else {
        /*使用默认的格式*/
        strftime(time_buffer, 30, default_time_format, info);

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
string format(const thread::id &t) {
    /*建立操作实例*/
    stringstream s_stream;
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
string format_message(const char *format, ...) {
    char buffer[INIT_MESSAGE_BUFFER];
    va_list va;
    va_start(va, format);
    /*格式化字符串*/
    vsnprintf(buffer, INIT_MESSAGE_BUFFER, format, va);
    va_end(va);
    return buffer;
}

/*按照格式格式化字符串
 * params format:格式化字符串
 * params args :参数列表
 * return:格式完成的数据
 * */
std::string format_message(const char *format, va_list args) {
    char buffer[INIT_MESSAGE_BUFFER];
    /*格式化字符串*/
    vsnprintf(buffer, INIT_MESSAGE_BUFFER, format, args);
    return buffer;
}

/*获取调用错误调用堆栈*/
string get_taceback() {
    /*设置栈调用函数*/
    string trace_back = "Traceback:\n";
    /*判定模式*/
    /*获取调用栈*/
    boost::stacktrace::stacktrace stack = boost::stacktrace::stacktrace();
    for (int i = 4; i < stack.size() - 3; i++) {
        /*获取调用栈*/
        trace_back += "  FILE " + stack[i].source_file() + ",  line " +
                      to_string(stack[i].source_line()) + "\n";
        trace_back += "       " + stack[i].name() + "\n";
    }
    return trace_back;
}

/*释放非空指针*/
void gsh_free(void *p) {
    if (p != nullptr) {
        free(p);
    } else {
        p = nullptr;
    }
}