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

#ifndef DNFSD_NFS_AUTH_H
#define DNFSD_NFS_AUTH_H

#include "nfs/nfs_compound_base.h"

int nfs_rpc_req2client_cred(struct svc_req *req, nfs_client_cred_t *pcred);

#endif //DNFSD_NFS_AUTH_H
