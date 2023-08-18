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
#include <yaml-cpp/yaml.h>

#include "rpc/types.h"

#define MAXHOSTNAMELEN	64
#define GANESHA_PIDFILE_PATH "/var/run/ganesha/ganesha.pid"
#define GANESHA_CONFIG_PATH "/etc/ganesha/ganesha.conf"

#define GANESHA_VERSION "5.4"
#define VERSION_COMMENT "GANESHA file server is 64 bits compliant and supports NFS v3,4.0,4.1 (pNFS) and 9P"
#define _GIT_HEAD_COMMIT "4f8c484b4ad2672e7e7ffb122b67fc34d96dc799"
#define _GIT_DESCRIBE "V5.4-0-g4f8c484b4"

typedef struct __nfs_start_info {
    int dump_default_config;
    int lw_mark_trigger;
    bool drop_caps;
} nfs_start_info_t;

extern time_t nfs_ServerEpoch;
extern bool config_errors_fatal;
extern tirpc_pkg_params ntirpc_pp;
extern YAML::Node dnfs_config;

#endif //DNFS_DNFSD_H
