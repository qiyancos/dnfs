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

const std::string format(const sockaddr_storage &out_data);

/*将pid转为字符串
 * params t:任意类型字符串
 * return: 转化完成的字符串
 * */
std::string pid_to_string(const std::thread::id &t);

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
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int creat_directory(const std::string &judge_dir, std::string *error_info);

/*将时间戳转化为日志
 * params timeStamp:转化的时间戳
 * params format:转化的日期格式
 * return: 转化完成的日期字符串
 * */
std::string get_record_time(const time_t &timeStamp, const std::string &format);

/* 从一个结构体成员变量指针推断出结构体本身的地址 */
#define get_parent_struct_addr(addr, type, member) ({			\
	const typeof(((type *) 0)->member) * __mptr = (addr);	\
	(type *)((char *) __mptr - offsetof(type, member)); })

/*按照格式格式化字符串
 * params format:格式化字符串
 * params ... :参数
 * return:格式完成的数据
 * */
std::string format_message(const char *foramt,...);

#endif //UTILS_COMMON_UTILS_H

