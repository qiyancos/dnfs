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

#include <sstream>
#include <algorithm>
#include <regex>

#include "utils/common_utils.h"

using namespace std;

static bool _beauty = false;
static int _indent = 4;
static string _indent_str = "    ";
static map<const void *, int> print_depth;

/* 该函数用于设置format的格式，设置为True则会追加缩进和换行 */
void set_print_beauty(const bool beauty) {
    _beauty = beauty;
}

/* 设置美化输出情况下自动所进的空格数量，默认为4 */
[[maybe_unused]] void set_print_indent(int indent) {
    _indent = indent;
    _indent_str = string(indent, ' ');
}

/* 对输入的指定类型进行格式化，返回一个格式化后的字符串 */
#define AUTO_DEF_GEN1(Type, Left_Bracket, Right_Bracket) \
template<typename T>\
const string format(const Type<T>& out_data) {\
    stringstream out;\
    string out_value;\
    const void * hash_key = static_cast<const void*>(&out_data);\
    int indent = 0;\
    if (_beauty && print_depth.find(hash_key) != print_depth.end()) {\
        indent = print_depth[hash_key];\
    }\
    string indent_str(indent * _indent, ' ');\
    if (out_data.size() == 0) {\
        out << Left_Bracket << Right_Bracket;            \
        out >> out_value;                                \
        return out_value;\
    }\
    int index = 0;\
    for (auto item = out_data.begin();item != out_data.end(); item++){\
        string line_str = "";\
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
            line_str = string() + (_beauty ? "\n" : "") + indent_str + Right_Bracket;\
        } else {\
            line_str = _beauty ? ",\n" : ", ";\
        }\
        out << line_str;\
        index++;\
    }\
    out >> out_value;                                    \
    return out_value;\
}

AUTO_DEF_GEN1(vector, "[", "]")

AUTO_DEF_GEN1(set, "(", ")")

AUTO_DEF_GEN1(unordered_set, "(", ")")

AUTO_DEF_GEN1(list, "[", "]")

AUTO_DEF_GEN1(queue, "[", "]")

AUTO_DEF_GEN1(deque, "[", "]")

AUTO_DEF_GEN1(stack, "[", "]")

#define AUTO_DEF_GEN2(Type, Left_Bracket, Right_Bracket) \
template<typename T1, typename T2>\
const string format (const Type<T1, T2> &out_data) {     \
    stringstream out;\
    string out_value;\
    const void* hash_key = static_cast<const void*>(&out_data);\
    int indent = 0;\
    if (_beauty && print_depth.find(hash_key) != print_depth.end()) {\
        indent = print_depth[hash_key];\
    }\
    string indent_str(indent * _indent, ' ');\
    if (out_data.size() == 0) {\
        out << Left_Bracket << Right_Bracket;\
        out >> out_value;                                \
        return out_value;\
    }\
    int index = 0;\
    for (auto item = out_data.begin();item != out_data.end(); item++){\
        string line_str = "";\
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
            line_str = string("") + (_beauty ? "\n" : "") + indent_str + Right_Bracket;\
        } else {\
            line_str = _beauty ? ",\n" : ", ";\
        }\
        out << line_str;\
        index++;\
    }\
    out >> out_value;                                \
    return out_value;\
}

AUTO_DEF_GEN2(map, "{", "}")

AUTO_DEF_GEN2(unordered_map, "{", "}")

template<typename T1, typename T2, typename T3>
const string format(const priority_queue<T1, T2, T3> &out_data) {
    vector<T1> content;
    priority_queue<T1, T2, T3> out_data_copy = out_data;
    while (!out_data_copy.empty()) {
        content.push_back(out_data_copy.top());
        out_data_copy.pop();
    }
    return format(content);
}

template<typename T1, typename T2>
const string format(const pair<T1, T2> &out_data) {
    map<T1, T2> content;
    content[out_data.first] = out_data.second;
    return format(content);
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
        result.push_back(temp);

        /*去掉已分割的字符串,在剩下的字符串中进行分割*/
        strs = strs.substr(pos + step, strs.size());

        /*接着查询分隔符进行切割*/
        pos = strs.find(split);
    }
}

/*判断字符串是不是纯数字*/
bool judge_number(const string &judge_str) {
    /*出数字正则表达式*/
    regex regexStr("\\d+");
    /*返回匹配结果*/
    return regex_match(judge_str, regexStr);
}

/*给非空指针设置信息*/
void set_ptr_info(string *error_info, const string &error_content) {
    /*如果不是空指针，赋值错误信息*/
    if (error_info != nullptr) {
        *error_info = error_content;
    }
}

/*转为小写*/
void to_lower(string &str) {
    /*全转为小写*/
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}


/*转为大写*/
void to_upper(string &str) {
    /*全转为小写*/
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}