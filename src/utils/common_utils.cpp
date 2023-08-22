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
#include <sys/stat.h>
#include <iostream>
#include <experimental/filesystem>

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

/*给非空指针设置信息
 * params error_info:字符串指针
 * params error_content:设置的信息
 * return
 * */
void set_ptr_info(string *error_info, const string &error_content) {
    /*如果不是空指针，赋值错误信息*/
    if (error_info != nullptr) {
        *error_info = error_content;
    }
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
            set_ptr_info(error_info, e.what());
            /*创建错误返回失败*/
            return 1;
        }
    } else if (info.st_mode & S_IFDIR) {
        /*目录存在*/
        return 0;
    } else {
        /*设置错误信息*/
        set_ptr_info(error_info, "the path for saving log is not a directory");
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
    if(!format.empty())
    {
        /*转化时间格式*/
        strftime(time_buffer, 30, format.c_str(), info);
    }else{
        /*使用默认的格式*/
        strftime(time_buffer, 30, default_time_format, info);

        /*获取系统当前时间*/
        auto now = chrono::system_clock::now();

        /*通过不同精度获取相差的毫秒数*/
        uint64_t dis_millseconds = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count()
                                   - chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count() * 1000;
        /*添加毫秒数据*/
        sprintf(time_buffer,default_msec_format,time_buffer,(int)dis_millseconds);
    }

    return time_buffer;
}