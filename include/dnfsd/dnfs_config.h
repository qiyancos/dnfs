/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#ifndef DNFSD_DNFS_CONFIG_H
#define DNFSD_DNFS_CONFIG_H

#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

#include "dnfsd/dnfs_meta_data.h"

/* DNFS的启动配置信息，存放了相关的数据信息 */
typedef struct __nfs_start_info {
    int dump_default_config;
//    int lw_mark_trigger;
//    bool drop_caps;
} nfs_start_info_t;

/* DNFSD默认配置文件的结构体 */
extern YAML::Node dnfs_config;

// 服务启动配置信息
extern nfs_start_info_t nfs_start_info;

/* DNFS的核心参数，与配置有关 */
extern nfs_parameter_t nfs_param;

/* 用于打印当前系统使用的配置信息 */
void dump_config();

/* 该函数用于快速从配置文件中获取指定层级的数据，并将结果按照指定格式写入到变量中，返回成功标志 */
template<typename T>
int config_get(T& out, const YAML::Node& config,
               const std::vector<std::string>& key_list);

#endif //DNFSD_DNFS_CONFIG_H
