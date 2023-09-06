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

#include <vector>
#include<sys/stat.h>
#include<unistd.h>
#include "log/log_file.h"
#include "log/log.h"
#include "utils/common_utils.h"
#include "log/log_exception.h"
#include "utils/time_utils.h"

#define FILE_MODEL_644 (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
using namespace std;

/*默认构造函数*/
LogFile::LogFile() = default;

/*使用map统一相同的logfile设置路径：logfile对象字典*/
map<string, shared_ptr<LogFile>> LogFile::path_log_file_dict = {};
/*设置路径：属性，用来对照相同的路径，切割属性是否一样*/
map<string, string> LogFile::path_rotate_attr_dict = {};

/*解析建立数据
 * params config_str:日志文件配置信息
 * params module_n:模块名
 * params log_l:日志等级
 * return
 * */
void LogFile::generate_data(const string &config_str, const string &module_n,
                            const log_level_t &log_l) {
    /*先判断有没有文件设置参数
     * 获取参数切割下标*/
    size_t args_index = config_str.find('@');

    /*截取文件夹路径*/
    string dir_path = config_str.substr(0, args_index);

    /*数字判定正则表达式*/
    regex number_regex_str("\\d+");

    /*文件目录合法字符匹配*/
    regex path_regex_str("(/[\\w]*)*");

    /*如果存在文件切割设置*/
    if (args_index != string::npos) {
        /*获取参数*/
        string args_str = config_str.substr(args_index + 2,
                                            config_str.size() - args_index - 3);
        /*保存参数*/
        vector<string> args;
        /*切割参数*/
        split_str(args_str, ",", args);
        /*判断参数数目，如果不为3直接报错*/
        if (args.size() != 3) {
            throw LogException(
                    "The log path setting has and only three parameters such as (time,midnight,30),you set is %s",
                    args_str.c_str());
        }
        /*解析参数*/
        /*先设置保留的日志数目*/
        if (!judge_regex(args[2], number_regex_str)) {
            throw LogException(
                    "The size of the number of saved logs must be a positive integer,you set is %s",
                    args[2].c_str());
        }
        /*需要保留的日志数量*/
        backup_count = stoi(args[2]);
        /*判断时间或者大小,只能存在一个*/
        if (args[0] == "size") {
            /*设置切割模式为按大小*/
            rotate_type = SIZE;
            /*不带单位报错,需要将其他单位转换为字节，根据严格规范后两位一定是单位 KB,MB,GB*/
            /*获取单位*/
            string unit_b = args[1].substr(args[1].size() - 2, args[1].size());
            /*单位转为小写*/
            to_lower(unit_b);

            /*获取数目*/
            string size_str = args[1].substr(0, args[1].size() - 2);
            /*如果是数字，转为数字*/
            if (!judge_regex(size_str, number_regex_str)) {
                throw LogException(
                        "Set the file size of the cut log setting must be a positive integer,you set is %s",
                        size_str.c_str());
            }
            /*转换数字*/
            log_limit.size_limit = stoll(size_str);
            /*判断单位*/
            if (unit_b == "kb") {
                log_limit.size_limit <<= 10;
            } else if (unit_b == "mb") {
                log_limit.size_limit <<= 20;
            } else if (unit_b == "gb") {
                if (log_limit.size_limit > 3) {
                    throw LogException(
                            "Split logs by size must be less than 4gb,you set is %d GB",
                            log_limit.size_limit);
                }
                log_limit.size_limit <<= 30;
            } else {
                throw LogException(
                        "Cutting logs by size must be selected among (kb,mb,gb), and case is ignored,you set is %s",
                        unit_b.c_str());
            }
        } else if (args[0] == "time") {
            /*设置切割模式为按时间*/
            rotate_type = TIME;
            /*日期模式转为大写*/
            to_upper(args[1]);
            if (args[1] == "NEVER") {
                /*设置不切割参数*/
                when = NEVER;
                log_limit.when_interval = 0;
            } else if (args[1] == "SECOND") {
                /*设置按秒切割*/
                when = SECOND;
                log_limit.when_interval = 1;
            } else if (args[1] == "MINUTE") {
                /*设置按分钟切割*/
                when = MINUTE;
                log_limit.when_interval = 60;
            } else if (args[1] == "HOUR") {
                /*设置按小时切割*/
                when = HOUR;
                log_limit.when_interval = 3600;
            } else if (args[1] == "DAY") {
                /*设置按天分割*/
                when = DAY;
                log_limit.when_interval = 86400;
            } else if (args[1] == "MIDNIGHT") {
                /*设置午夜切割*/
                when = MIDNIGHT;
                log_limit.when_interval = 86400;
            } else if (args[1] == "WEEK") {
                /*设置按周切割*/
                when = WEEK;
                log_limit.when_interval = 604800;
            } else {
                throw LogException(
                        "Cutting logs by time must be selected in the list below:\n"
                        "        NEVER,\n"
                        "        SECOND,\n"
                        "        MINUTE,\n"
                        "        HOUR,\n"
                        "        DAY,\n"
                        "        MIDNIGHT,\n"
                        "        WEEK\n"
                        "and case is ignored,you set is %s", args[1].c_str());
            }
        } else {
            throw LogException(
                    "The log cutting limit parameter can only be selected between 'size' and 'time',you set is %s",
                    args[0].c_str());
        }
    }
    /*如果路径不合法*/
    if (!judge_regex(dir_path, path_regex_str)) {
        throw LogException(
                "The log directory must be an absolute path and can only be named with numbers, letters and '_',you set is %s",
                dir_path.c_str());
    }
    /*查看路径是否存在，不存在创建,创建错误直接返回*/
    if (creat_directory(dir_path, nullptr) != 0) {
        throw LogException(
                "The storage log path '%s' set is not a directory",
                dir_path.c_str());
    }
    /*判断是否有写权限*/
    if (access(dir_path.c_str(), W_OK) != 0) {
        throw LogException(
                "The directory '%s' where the log file is saved does not have write permissions",
                dir_path.c_str());
    }
    /*路径赋值*/
    log_directory_path = dir_path;
    /*设置模块名*/
    module_name = module_n;
    /*设置日志等级*/
    log_level = log_l;
    /*生成初始日志文件路径*/
    generate_log_path();
}

/*输出日志信息
 * params message:日志信息
 * return
 * */
void LogFile::out_message(const string &message) {
    /*生成文件句柄等写入文件操作的实例*/
    switch (rotate_type) {
        case 0:
            /*不切割日志*/
            not_rotate();
            break;
        case 1:
            /*按照时间切割*/
            rotate_by_time();
            break;
        case 2:
            /*按照大小切割*/
            rotate_by_size();
            break;
    }
    /*写文件*/
    ssize_t result = write(file_handler, message.c_str(), size(message));
    /*写入出错抛出异常*/
    if (result == -1) {
        throw LogException(
                "Failed to write information to file '%s' with module '%s' output level '%s'",
                log_file_path.c_str(), module_name.c_str(),
                log_level_info_dict[log_level].first[0].c_str());
    }
}

/*生成日志文件路径
 * return
 * */
void LogFile::generate_log_path() {
    /*首先加上程序名*/
    file_name = logger.program_name;
    /*然后判断模块名*/
    switch (logger.get_log_generate()) {
        case L_JUST_ONE:
            break;
        case L_MODULE:
            /*拼接模块名*/
            file_name += ("_" + module_name);
            break;
        case L_LOG_LEVEL:
            /*拼接日志等级*/
            file_name += ("_" + log_level_info_dict[log_level].first[0]);
            break;
        case LMODULE_LOG_LEVEL:
            /*拼接模块名和日志等级*/
            file_name += ("_" + module_name);
            file_name += ("_" + log_level_info_dict[log_level].first[0]);
            break;
    }
    /*判断切割类型*/
    switch (rotate_type) {
        case 0:
            break;
        case 1:
            /*设置时间*/
            file_name += "_time";
            break;
        case 2:
            /*设置大小*/
            file_name += "_size";
            break;
    }
    /*最后拼接log*/
    file_name += ".log";
    /*不切割只使用模块名和日志等级*/
    log_file_path = log_directory_path + "/" + file_name;
}

/*不切割日志
 * return
 * */
void
LogFile::not_rotate() {
    /*判断并生成日志文件*/
    judge_and_create_log_file();
}

/*按时间切割数据方法
 * return
 * */
void LogFile::rotate_by_time() {
    /*获取现在的时间*/
    time_t now_time = time(nullptr);

    /*如果已经建立了写入日志，并且超过了时间限制,进行切割*/
    if (use_file_build_time != 0 and
        (now_time - use_file_build_time) > log_limit.when_interval) {
        /*进行午夜判定*/
        if (when != MIDNIGHT or get_mid_night(now_time) == now_time) {
            /*切割文件*/
            rotate_log_file(now_time);
        }
    }

    /*判断并生成日志文件*/
    judge_and_create_log_file();
}

/*按大小切割数据方法
 * return
 * */
void LogFile::rotate_by_size() {
    /*文件信息保存*/
    struct stat log_file_stat = {};
    /*获取文件信息*/
    fstat(file_handler, &log_file_stat);
    /*获取现在的时间*/
    time_t now_time = time(nullptr);

    /*对比文件的大小*/
    if (log_file_stat.st_size > log_limit.size_limit) {
        /*切割文件*/
        rotate_log_file(now_time);
    }

    /*判断并生成日志文件*/
    judge_and_create_log_file();

}

/*判断并生成日志文件*/
void LogFile::judge_and_create_log_file() {
    /*如果没有获取文件路径，直接报错*/
    if (log_file_path.empty()) {
        throw LogException(
                "To obtain the log path under the log level '%s' of the module '%s'",
                module_name.c_str(),
                log_level_info_dict[log_level].first[0].c_str());
    }
    /*创建日志文件并获取句柄*/
    if (access(log_file_path.c_str(), F_OK) != 0) {
        /*尝试打开文件*/
        file_handler = open(log_file_path.c_str(), O_CREAT | O_WRONLY,
                            FILE_MODEL_644);
        /*如果文件流没有成功打开*/
        if (file_handler == -1) {
            throw LogException(
                    "Failed to create a log file '%s' for this module '%s' at this log level %s",
                    log_file_path.c_str(),
                    module_name.c_str(),
                    log_level_info_dict[log_level].first[0].c_str());
        }
        /*建立构造时间*/
        use_file_build_time = time(nullptr);

    }
    /*文件存在，但没有获取输出文件流，获取*/
    if (file_handler == -1) {
        /*尝试打开文件,追加方式*/
        file_handler = open(log_file_path.c_str(), O_WRONLY | O_APPEND);
        /*如果文件流没有成功打开*/
        if (file_handler == -1) {
            log_file_path = "";
            throw LogException(
                    "Failed to open the log file '%s' under this log level '%s' of this module '%s'",
                    log_file_path.c_str(),
                    log_level_info_dict[log_level].first[0].c_str(),
                    module_name.c_str());
        }
        /*获取文件最后改变的时间作为创建时间*/
        struct stat file_stat = {};
        fstat(file_handler, &file_stat);
        use_file_build_time = file_stat.st_ctim.tv_sec;
    }
    /*如果没有写权限*/
    if (access(log_file_path.c_str(), W_OK) != 0) {
        /*如果保存了文件流关闭*/
        if (file_handler > -1) {
            close(file_handler);
        }
        throw LogException(
                "The log files '%s' under this log level '%s' of this module '%s' do not have write permission",
                log_file_path.c_str(),
                log_level_info_dict[log_level].first[0].c_str(),
                module_name.c_str());
    }
}

/*适应单独更新数据
 * params use_module_name:模块名
 * return
 * */
void LogFile::set_module_name(const string &use_module_name) {
    module_name = use_module_name;
    /*重新生成日志名称*/
    generate_log_path();
}

/*读取日志目录下的所有文件
 * return 获取的文件字符串
 * */
string LogFile::get_dir_file() {
    /*文件名拼接字符串*/
    string file_str;

    /*设置文件目录读取指针*/
    DIR *dirp;

    /*设置文件读取指针*/
    struct dirent *dir_entry;

    /*读取日志目录*/
    if ((dirp = opendir(log_directory_path.c_str())) == nullptr) {
        return file_str;
    }

    /*遍历目录*/
    while ((dir_entry = readdir(dirp)) != nullptr) {
        /*判断是不是文件*/
        if (dir_entry->d_type == DT_REG) {
            /*添加结果*/
            file_str += ":";
            file_str += dir_entry->d_name;
        }
    }
    /*关闭文件目录句柄*/
    closedir(dirp);
    /*返回结果*/
    return file_str;
}

/*获取切割文件编号
 * params search_file_name:需排序的文件名
 * return 获取的文件名
 * */
string
LogFile::get_file_number(const string &search_file_name) {
    /*获取目录下所有的文件名*/
    string file_str = get_dir_file();

    /*如果没有获取文件列表,直接返回1*/
    if (file_str.empty()) {
        return "1";
    }

    /*进行正则遍历*/
    /*正则结果保存*/
    smatch file_match;

    /*设置正则匹配规则*/
    regex pattern(search_file_name + ".\\d+");

    /*遍历查找数据*/
    /*设置查询范围*/
    string::const_iterator iter_begin = file_str.cbegin();
    string::const_iterator iter_end = file_str.cend();

    /*文件序号*/
    long int file_number = 0;

    /*开始查找*/
    while (regex_search(iter_begin, iter_end, file_match, pattern)) {
        /*分割结果保存*/
        vector<string> result;
        /*用文件名切割数据*/
        split_str(file_match[0].str(), search_file_name + '.', result);

        /*获取标号*/
        long int now_number = strtol(result[0].c_str(), nullptr, 0);
        /*获取最大的标号*/
        if (now_number > file_number) {
            file_number = now_number;
        }
        /*更新迭代器位置*/
        iter_begin = file_match[0].second;
    }
    return to_string(file_number + 1);
}

/*获取午夜的时间戳
 * return 获取的午夜时间
 * */
time_t LogFile::get_mid_night(const time_t &now_time) {
    struct tm *tm = localtime(&now_time);
    /*将小时之下的时间置为0*/
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    return mktime(tm);
}

/*切割文件重命名，时间切割使用对应的时间模板重命名，大小则使用编号
 * params now_time:记录的时间
 * return
 * */
void LogFile::rotate_log_file(const time_t &now_time) {
    /*先关闭之前的文件*/
    if (file_handler > -1) {
        close(file_handler);
    }
    /*构造文件名*/
    string mv_file_name;

    switch (rotate_type) {
        case 1:
            /*按照时间切割，以时间为后缀*/
            mv_file_name =
                    file_name + "." +
                    format(now_time, 0, time_format[when]);
            break;
        default:
            /*按照日期切割或者其他，文件编号为后缀*/
            mv_file_name =
                    file_name + "." + get_file_number(file_name);
            break;

    }
    /*生成切分的日志文件名称，当前的日期加上编号*/
    string rotate_path = log_directory_path + "/" + mv_file_name;
    /*将日志文件重命名*/
    rename(log_file_path.c_str(), rotate_path.c_str());

    /*记录新的文件路径*/
    file_path_list.push_back(rotate_path);

    /*日志文件总数加1*/
    log_files += 1;

    /*判断日志文件数目,超过限制删除*/
    if (backup_count > 0 and log_files > backup_count) {
        /*删除文件路径记录的第一条数据*/
        if (remove(file_path_list[0].c_str()) != 0) {
            throw LogException("Failed to delete module '%s' log file '%s'",
                               module_name.c_str(), file_path_list[0].c_str());
        }
        /*移除路径记录*/
        file_path_list.erase(file_path_list.begin());
        /*文件记录数目减一*/
        log_files -= 1;
    }
}

/*判定生成日志文件对象
 * params config_str:日志文件配置信息
 * params module_n:模块名
 * params log_l:日志等级
 * return: 获取的日志对象
 * */
shared_ptr<LogFile>
LogFile::get_log_file(const string &config_str, const string &module_n,
                      const log_level_t &log_l) {
    /*先只简单的截取路径和参数和保存的字典进行对比*/
    /*先判断有没有文件设置参数
     * 获取参数切割下标*/
    size_t args_index = config_str.find('@');

    /*建立设置参数*/
    string args_str;

    /*如果存在文件切割设置*/
    if (args_index != string::npos) {
        /*获取参数*/
        args_str = config_str.substr(args_index + 2,
                                     config_str.size() - args_index - 3);
    }
    /*截取文件夹路径*/
    string dir_path = config_str.substr(0, args_index);

    /*生成日志文件名*/
    /*先建立文件属性对象*/
    auto *log_file = new LogFile();
    /*建立属性,有错误将抛出异常*/
    log_file->generate_data(config_str, module_n, log_l);

    /*获取文件名*/
    string log_file_path = log_file->get_log_path();

    /*获取是否建立的文件*/
    if (path_log_file_dict.find(log_file_path) == path_log_file_dict.end()) {
        /*建立智能指针*/
        shared_ptr<LogFile> log_f;
        log_f.reset(log_file);
        /*保存数据*/
        path_log_file_dict[log_file_path] = log_f;
        path_rotate_attr_dict[log_file_path] = args_str;

    } else {
        /*已经存在判断属性设置是否一样，不一样直接抛出异常*/
        if (path_rotate_attr_dict.find(log_file_path)->second != args_str) {
            throw LogException(
                    "Module '%s' log file '%s' cutting mode setting conflict",
                    module_n.c_str(), log_file_path.c_str());
        }
    }
    /*返回logfile对象指针*/
    return path_log_file_dict[log_file_path];
}

/*获取日志路径
 * return: 日志路径
 * */
string LogFile::get_log_path() {
    return log_file_path;
}

/*析构函数*/
LogFile::~LogFile() {
    /*关闭文件指针*/
    if (file_handler > -1) {
        close(file_handler);
    }
}



