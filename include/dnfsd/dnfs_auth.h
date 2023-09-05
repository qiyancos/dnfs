/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 * Contributeur : Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
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

#ifndef DNFSD_DNFS_AUTH_H
#define DNFSD_DNFS_AUTH_H

#include "dnfsd/dnfs_meta_data.h"

enum auth_stat svc_auth_authenticate(struct svc_req *req, bool *no_dispatch);

const char *auth_stat2str(enum auth_stat why);

#endif //DNFSD_DNFS_AUTH_H
