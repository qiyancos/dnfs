//
// Created by iecas on 2023/8/10.
//

#ifndef DNFS_DNFSD_H
#define DNFS_DNFSD_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAXHOSTNAMELEN	64

#define GANESHA_PIDFILE_PATH "/var/run/ganesha/ganesha.pid"
#define GANESHA_CONFIG_PATH "/etc/ganesha/ganesha.conf"

typedef struct __nfs_start_info {
    int dump_default_config;
    int lw_mark_trigger;
    bool drop_caps;
} nfs_start_info_t;

#define GANESHA_VERSION "5.4"
#define VERSION_COMMENT "GANESHA file server is 64 bits compliant and supports NFS v3,4.0,4.1 (pNFS) and 9P"
#define _GIT_HEAD_COMMIT "4f8c484b4ad2672e7e7ffb122b67fc34d96dc799"
#define _GIT_DESCRIBE "V5.4-0-g4f8c484b4"

#endif //DNFS_DNFSD_H
