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

#include <experimental/filesystem>

#include "dnfsd/dnfs_config.h"

using namespace std;

#define MODULE_NAME "config"

/* DNFSD全局默认的YAML文件结构 */
YAML::Node dnfs_config;

// 服务启动配置信息
nfs_start_info_t nfs_start_info = {
    .dump_default_config = false
//    .lw_mark_trigger = false,
//    .drop_caps = true
};

/* DNFS的核心参数，与配置有关 */
nfs_parameter_t nfs_param;

/* 默认配置文件的字符表达形式 */
static const char default_config[] =
        "# default config file content\n"
        "log:\n"
        "    limit_type: time\n"
        "    limit_info: MIDNIGHT\n"
        "    backup_count: 30\n"
        "    formatter: \"%(levelname) <%(asctime)><PID-%(process)>: %(message)\"\n"
        "    path: /var/log/dnfsd/runtime.log\n";

/* 创建一个默认初始化的配置 */
static void init_default_config(const string& config_file_path) {
    string config_dir = config_file_path.substr(0,
                                                config_file_path.find_last_of('/'));
    try {
        experimental::filesystem::create_directories(config_dir);
    } catch (exception& e) {
        fprintf(stderr,
                "Failed to create directory \"%s\" for config file: %s.\n",
                config_dir.c_str(), e.what());
        exit(-1);
    }

    ofstream config_file_os;
    try {
        config_file_os.open(config_file_path.c_str(), ios::out);
    } catch (exception& e) {
        fprintf(stderr,
                "Failed to create default config file \"%s\"\n",
                config_file_path.c_str());
        exit(-1);
    }
    config_file_os << default_config;
    config_file_os.close();
}

/* 初始化配置文件并进行解析 */
void init_config(const string& config_file_path) {
    fprintf(stdout, "Start init base config file\n");
    if (access(config_file_path.c_str(), F_OK) == -1) {
        /* 如果目标文件不存在，创建一个默认初始化的配置 */
        init_default_config(config_file_path);
    }
    try {
        dnfs_config = YAML::LoadFile(config_file_path);
    } catch (exception& e) {
        fprintf(stderr, "Failed to load config file %s: %s\n",
                config_file_path.c_str(), e.what());
        exit(-1);
    }
    /* 这里还会做一些基本配置文件的校验 */
    /*TODO*/
}

/* 用于打印当前系统使用的配置信息 */
void dump_config() {
    /*TODO*/
}