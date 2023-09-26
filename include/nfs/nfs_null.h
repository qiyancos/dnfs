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

#ifndef DNFSD_NFS_NULL_H
#define DNFSD_NFS_NULL_H
union nfs_arg_t;
union nfs_res_t;
/* NFS NULL Process function*/
int nfs_null([[maybe_unused]] nfs_arg_t *arg,
             [[maybe_unused]] struct svc_req *req,
             [[maybe_unused]] nfs_res_t *res);

/* NFS FREE Process Function */
void nfs_null_free([[maybe_unused]] nfs_res_t *res);

#endif //DNFSD_NFS_NULL_H
