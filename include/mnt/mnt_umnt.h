/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#ifndef DNFSD_MNT_UMNT_H
#define DNFSD_MNT_UMNT_H

/*声明数据参数*/
union nfs_arg_t;
union nfs_res_t;

int mnt_umnt(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);
void mnt_umnt_free(nfs_res_t *res);


#endif //DNFSD_MNT_UMNT_H
