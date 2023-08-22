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

/* 该函数用于设置format的格式，设置为True则会追加缩进和换行 */
void set_print_beauty(const bool beauty);

/* 设置美化输出情况下自动所进的空格数量，默认为4 */
[[maybe_unused]] void set_print_indent(int indent);

template<typename T>
inline const std::string format(const T& out_data) {
    return std::to_string(out_data);
}

inline const std::string format(const std::string& out_data) {
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
void split_str(const std::string &str, const std::string &split, std::vector<std::string> &result);

/*判断字符串是不是纯数字*/
bool judge_number(const std::string &judge_str);

/*给非空指针设置信息*/
void set_ptr_info(std::string *error_info, const std::string &error_content);

/*转为小写*/
void to_lower(std::string &str);

/*转为大写*/
void to_upper(std::string &str);

/* 从一个结构体成员变量指针推断出结构体本身的地址 */
#define get_parent_struct_addr(addr, type, member) ({			\
	const typeof(((type *) 0)->member) * __mptr = (addr);	\
	(type *)((char *) __mptr - offsetof(type, member)); })

#endif //UTILS_COMMON_UTILS_H