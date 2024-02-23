/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#ifndef DNFSD_DNFS_CONFIG_H
#define DNFSD_DNFS_CONFIG_H

#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

#include "log/log.h"
#include "utils/common_utils.h"
#include "dnfsd/dnfs_meta_data.h"

/* DNFS的启动配置信息，存放了相关的数据信息 */
typedef struct __nfs_start_info {
    int dump_default_config;
//    int lw_mark_trigger;
//    bool drop_caps;
} nfs_start_info_t;

// 服务启动配置信息
extern nfs_start_info_t nfs_start_info;

/* DNFS的核心参数，与配置有关 */
extern nfs_parameter_t nfs_param;

/* 日志相关的配置信息 */
struct dnfs_logging_config {
    /* 日志文件的绝对路径 */
    std::string path;

    /* 日志文件的rotate类型，可以是size或time */
    std::string limit_type;

    /* 日志文件的rotate信息 */
    std::string limit_info;

    /* 日志文件rotate后保留的历史文件个数 */
    int backup_count = 30;

    /* 日志文件的基本输出格式 */
    std::string formatter = "%(levelname) <%(asctime)><PID-%(process)>: %(message)";
};

/* 运行时的性能配置 */
struct dnfs_running_config {
    /* 线程池的大小，0表示没有限制 */
    int max_thead_size = 0;
};

/* DNFS配置 */
struct dnfs_runtime_config {
    /* 日志相关配置 */
    dnfs_logging_config log_config;

    /* 运行时相关配置 */
    dnfs_running_config run_config;
};

extern dnfs_runtime_config dnfs_config;

/* 初始化配置文件 */
void init_config(const std::string& config_file_path);

/* 初始化日志配置 */
int init_logging_config(dnfs_logging_config& out);

/* 用于打印当前系统使用的配置信息 */
void dump_config();

#endif //DNFSD_DNFS_CONFIG_H
