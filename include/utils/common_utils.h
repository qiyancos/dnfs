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

#ifndef UTILS_COMMON_UTILS_H
#define UTILS_COMMON_UTILS_H

#include <sys/socket.h>

#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <fstream>
#include <stack>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <set>
#include <regex>
#include <sstream>
#include <thread>

#define SET_PTR_INFO(error_info, error_content) if(error_info != nullptr) *error_info = error_content;
/*设置格式化缓存区默认大小*/
#define ONE_MB 1024

extern bool _beauty;
extern int _indent;
extern std::string _indent_str;
extern std::map<const void *, int> print_depth;

/* Allow much more space than we really need for a sock name. An IPV4 address
 * embedded in IPv6 could use 45 bytes and then if we add a port, that would be
 * an additional 6 bytes (:65535) for a total of 51, and then one more for NUL
 * termination. We could use 64 instead of 128.
 */
#define SOCK_NAME_MAX 128

/* 该函数用于设置format的格式，设置为True则会追加缩进和换行 */
void set_print_beauty(bool beauty);

/* 设置美化输出情况下自动所进的空格数量，默认为4 */
[[maybe_unused]] void set_print_indent(int indent);

template<typename T>
inline std::string format(const T &out_data) {
    return std::to_string(out_data);
}

inline std::string format(const std::string &out_data) {
    return out_data;
}

std::string format(sockaddr_storage* out_data);

/*将pid转为字符串
 * params t:任意类型字符串
 * return: 转化完成的字符串
 * */
std::string format(const std::thread::id &t);

/* 对输入的指定类型进行格式化，返回一个格式化后的字符串 */
#define AUTO_DEF_GEN1(Type, Left_Bracket, Right_Bracket) \
template<typename T>\
std::string format(const Type<T>& out_data) {\
    std::stringstream out;\
    const void * hash_key = static_cast<const void*>(&out_data);\
    int indent = 0;\
    if (_beauty && print_depth.find(hash_key) != print_depth.end()) {\
        indent = print_depth[hash_key];\
    }\
    std::string indent_str(indent * _indent, ' ');\
    if (out_data.empty()) {\
        out << Left_Bracket << Right_Bracket;            \
        return out.str();\
    }\
    unsigned int index = 0;\
    for (auto item = out_data.begin();item != out_data.end(); item++){\
        std::string line_str;\
        if (item == out_data.begin()) {\
            line_str += _beauty ? Left_Bracket"\n" : Left_Bracket;\
        }\
        line_str += _beauty ? indent_str + _indent_str: "";\
        out << line_str;\
        \
        const void* sub_item_hash_key = static_cast<const void*>(&item);\
        print_depth[sub_item_hash_key] = indent + 1;\
        out << format(*item);\
        print_depth.erase(sub_item_hash_key);\
        \
        if (index == out_data.size() - 1) {\
            line_str = std::string() + (_beauty ? "\n" : "") + indent_str + Right_Bracket;\
        } else {\
            line_str = _beauty ? ",\n" : ", ";\
        }\
        out << line_str;\
        index++;                                         \
    }\
    return out.str();\
}

AUTO_DEF_GEN1(std::vector, "[", "]")

AUTO_DEF_GEN1(std::set, "(", ")")

AUTO_DEF_GEN1(std::unordered_set, "(", ")")

AUTO_DEF_GEN1(std::list, "[", "]")

AUTO_DEF_GEN1(std::queue, "[", "]")

AUTO_DEF_GEN1(std::deque, "[", "]")

AUTO_DEF_GEN1(std::stack, "[", "]")

#define AUTO_DEF_GEN2(Type, Left_Bracket, Right_Bracket) \
template<typename T1, typename T2>\
std::string format (const Type<T1, T2> &out_data) {     \
    std::stringstream out;\
    const void* hash_key = static_cast<const void*>(&out_data);\
    int indent = 0;\
    if (_beauty && print_depth.find(hash_key) != print_depth.end()) {\
        indent = print_depth[hash_key];\
    }\
    std::string indent_str(indent * _indent, ' ');\
    if (out_data.size() == 0) {\
        out << Left_Bracket << Right_Bracket;\
        return out.str();\
    }\
    int index = 0;\
    for (auto item = out_data.begin();item != out_data.end(); item++){\
        std::string line_str;\
        if (item == out_data.begin()) {\
            line_str += _beauty ? Left_Bracket"\n" : Left_Bracket;\
        }\
        line_str += _beauty ? indent_str + _indent_str : "";\
        out << line_str;\
        \
        const T1& key = item->first;\
        bool old_beauty = _beauty;\
        set_print_beauty(false);\
        out << format(key) << ": ";\
        set_print_beauty(old_beauty);\
        \
        const T2& value = item->second;\
        const void* sub_item_hash_key = static_cast<const void*>(&value);\
        print_depth[sub_item_hash_key] = indent + 1;\
        out << format(value);\
        print_depth.erase(sub_item_hash_key);\
        \
        if (index == out_data.size() - 1) {\
            line_str = std::string("") + (_beauty ? "\n" : "") + indent_str + Right_Bracket;\
        } else {\
            line_str = _beauty ? ",\n" : ", ";\
        }\
        out << line_str;\
        index++;\
    }\
    return out.str();\
}


AUTO_DEF_GEN2(std::map, "{", "}")

AUTO_DEF_GEN2(std::unordered_map, "{", "}")

template<typename T1, typename T2, typename T3>
std::string format(const std::priority_queue<T1, T2, T3> &out_data) {
    std::vector<T1> content;
    std::priority_queue<T1, T2, T3> out_data_copy = out_data;
    while (!out_data_copy.empty()) {
        content.push_back(out_data_copy.top());
        out_data_copy.pop();
    }
    return format(content);
}

template<typename T1, typename T2>
std::string format(const std::pair<T1, T2> &out_data) {
    std::map<T1, T2> content;
    content[out_data.first] = out_data.second;
    return format(content);
}

/*切割字符串*/
void split_str(const std::string &str, const std::string &split,
               std::vector<std::string> &result);

/*验证字符串是否满足正则表达式
 * params judge_str:验证的字符串
 * params judge_str:验证的正则表达式
 * return: true 匹配成功 false 匹配失败
 * */
bool judge_regex(const std::string &judge_str,
                 const std::regex &regex_expression);

/*转为小写
 * params str:需转换的字符串
 * return
 * */
void to_lower(std::string &str);

/*转为大写
 * params str:需转换的字符串
 * return
 * */
void to_upper(std::string &str);

/*判断文件目录是否存在
 * params judge_dir:验证的路径
 * return:
 * */
void creat_directory(const std::string &judge_dir);

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
 * params timeStamp:转化的时间戳
 * params dis_millseconds:转化的毫秒
 * params format:转化的日期格式
 * return: 转化完成的日期字符串
 * */
std::string format(const time_t &time_stamp,const uint64_t &dis_millseconds, const std::string &format);

/* 从一个结构体成员变量指针推断出结构体本身的地址 */
#define get_parent_struct_addr(addr, type, member) ({			\
	const typeof(((type *) 0)->member) * __mptr = (addr);	\
	(type *)((char *) __mptr - offsetof(type, member)); })

/*按照格式格式化字符串
 * params format:格式化字符串
 * params ... :参数
 * return:格式完成的数据
 * */
std::string format_message(const char *format,...);

/*按照格式格式化字符串
 * params format:格式化字符串
 * params args :参数列表
 * return:格式完成的数据
 * */
std::string format_message(const char *format,va_list args);

/*获取调用错误调用堆栈*/
std::string get_taceback();
#endif //UTILS_COMMON_UTILS_H

