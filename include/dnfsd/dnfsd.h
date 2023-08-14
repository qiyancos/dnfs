//
// Created by iecas on 2023/8/10.
//

#ifndef DNFS_DNFSD_H
#define DNFS_DNFSD_H

#include <string.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAXHOSTNAMELEN	64

typedef struct __nfs_start_info {
    int dump_default_config;
    int lw_mark_trigger;
    bool drop_caps;
} nfs_start_info_t;

#endif //DNFS_DNFSD_H
