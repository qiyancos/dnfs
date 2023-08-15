/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 * Contributor(nfs-ganesha): Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
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

#include "rpc/types.h"

#define MAXHOSTNAMELEN	64

typedef struct __nfs_start_info {
    int dump_default_config;
    int lw_mark_trigger;
    bool drop_caps;
} nfs_start_info_t;

extern time_t nfs_ServerEpoch;
extern bool config_errors_fatal;
extern tirpc_pkg_params ntirpc_pp;

void init_crash_handlers(void);

#endif //DNFS_DNFSD_H
