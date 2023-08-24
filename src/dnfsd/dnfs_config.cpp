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

#include "dnfsd/dnfs_config.h"

using namespace std;

#define MODULE_NAME("config")

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

/* 用于打印当前系统使用的配置信息 */
void dump_config() {
    /*TODO*/
}