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

/* 该函数用于设置format的格式，设置为True则会追加缩进和换行 */
void set_print_beauty(const bool beauty);

/* 设置美化输出情况下自动所进的空格数量，默认为4 */
[[maybe_unused]] void set_print_indent(int indent);

template<typename T>
inline const std::string format(const T &out_data) {
    return std::to_string(out_data);
}

inline const std::string format(const std::string &out_data) {
    return out_data;
}

/* 对输入的指定类型进行格式化，返回一个格式化后的字符串 */
#define AUTO_DECL_GEN1(Type, Left_Bracket, Right_Bracket) \
template<typename T>\
const std::string format(const Type<T>& out_data);

/* 对常见的迭代器类别进行声明生成 */
AUTO_DECL_GEN1(std::vector, "[", "]")

AUTO_DECL_GEN1(std::set, "(", ")")

AUTO_DECL_GEN1(std::unordered_set, "(", ")")

AUTO_DECL_GEN1(std::list, "[", "]")

AUTO_DECL_GEN1(std::queue, "[", "]")

AUTO_DECL_GEN1(std::deque, "[", "]")

AUTO_DECL_GEN1(std::stack, "[", "]")

/* 对输入的指定类型进行格式化，返回一个格式化后的字符串 */
#define AUTO_DECL_GEN2(Type, Left_Bracket, Right_Bracket) \
template<typename T1, typename T2>\
const std::string format (const Type<T1, T2> &out_data);

/* 对常见的迭代器类别进行声明生成 */
AUTO_DECL_GEN2(std::map, "{", "}")

AUTO_DECL_GEN2(std::unordered_map, "{", "}")

/* 对输入的指定类型进行格式化，返回一个格式化后的字符串 */
template<typename T1, typename T2, typename T3>
const std::string format(const std::priority_queue<T1, T2, T3> &out_data);

template<typename T1, typename T2>
const std::string format(const std::pair<T1, T2> &out_data);

/*切割字符串*/
void split_str(const std::string &str, const std::string &split,
               std::vector<std::string> &result);

/*验证字符串是否满足正则表达式
 * params judge_str:验证的字符串
 * params judge_str:验证的正则表达式
 * return: true 匹配成功 false 匹配失败
 * */
bool
judge_regex(const std::string &judge_str, const std::regex &regex_expression);

/*给非空指针设置信息
 * params error_info:字符串指针
 * params error_content:设置的信息
 * return
 * */
void set_ptr_info(std::string *error_info, const std::string &error_content);

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

#endif //UTILS_COMMON_UTILS_H