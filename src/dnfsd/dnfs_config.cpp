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
YAML::Node dnfs_yaml_config;

/* DNFSD的全局配置参数属性结构体 */
dnfs_runtime_config dnfs_config;

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

/* 解析日志配置 */
int init_logging_config(dnfs_logging_config& out) {
    try {
        YAML::Node log_node = dnfs_yaml_config["log"];
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log node not found in config file: %s\n",
                e.what());
        return -1;
    }

    try {
        out.path = dnfs_yaml_config["log"]["path"].as<string>();
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log.path missing\n");
    }

    try {
        out.limit_type = dnfs_yaml_config["log"]["limit_type"].as<string>();
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log.limit_type missing\n");
    }

    try {
        out.limit_info = dnfs_yaml_config["log"]["limit_info"].as<string>();
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log.limit_info missing\n");
    }

    try {
        out.backup_count = dnfs_yaml_config["log"]["backup_count"].as<int>();
    } catch (YAML::InvalidNode e) {
        fprintf(stderr, "Warn: log.backup_count missing\n");
    } catch (YAML::BadConversion e) {
        fprintf(stderr, "Error: log.backup_count \"%s\" format is not int\n",
                dnfs_yaml_config["log"]["backup_count"].as<string>().c_str());
        return -1;
    }

    try {
        out.formatter = dnfs_yaml_config["log"]["formatter"].as<string>();
    } catch (YAML::InvalidNode) {
        fprintf(stderr, "Warn: log.formatter missing\n");
    }
    return 0;
}

/* 初始化配置文件并进行解析 */
void init_config(const string& config_file_path) {
    if (access(config_file_path.c_str(), F_OK) == -1) {
        /* 如果目标文件不存在，创建一个默认初始化的配置 */
        fprintf(stdout, "Start init default config file\n");
        init_default_config(config_file_path);
    }

    try {
        dnfs_yaml_config = YAML::LoadFile(config_file_path);
    } catch (exception& e) {
        fprintf(stderr, "Failed to load config file %s: %s\n",
                config_file_path.c_str(), e.what());
        exit(-1);
    }

    /* 首先从配置文件中读取log相关的配置 */
    fprintf(stdout, "Loading config for logging from config file\n");
    if (init_logging_config(dnfs_config.log_config)) {
        fprintf(stderr, "Failed to load log config\n");
        exit(-1);
    }
}

/* 用于打印当前系统使用的配置信息 */
void dump_config() {
    /*TODO*/
}