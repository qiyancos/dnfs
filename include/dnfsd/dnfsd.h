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

#ifndef DNFS_DNFSD_H
#define DNFS_DNFSD_H

#include <stdbool.h>

/* 主机名称的最大长度 */
#define MAXHOSTNAMELEN	64

/* 默认的DNFSD的PID文件记录路径，该文件用于标记服务进程，可用于判断服务启动状态和关闭服务 */
#define DNFSD_PIDFILE_PATH "/var/run/dnfsd/main.pid"

/* 默认的DNFSD的主配置文件路径 */
#define DNFSD_CONFIG_PATH "/etc/dnfsd/main.conf"

/* 当前的DNFSD的版本 */
#define DNFSD_VERSION "5.4"

/* 当前DNFSD的版本信息支持说明 */
#define VERSION_COMMENT "DNFSD file server is 64 bits compliant and supports NFS v3"

/* 当前的git提交版本hash */
#define _GIT_HEAD_COMMIT "4f8c484b4ad2672e7e7ffb122b67fc34d96dc799"

/* 当前最新提交的版本描述信息 */
#define _GIT_DESCRIBE "V5.4-0-g4f8c484b4"

/* TODO */
[[maybe_unused]] extern time_t nfs_ServerEpoch;

/* 针对配置文件严重错误是否退出进程 */
[[maybe_unused]] extern bool config_errors_fatal;

#endif //DNFS_DNFSD_H
