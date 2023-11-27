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

#ifndef DNFSD_FILE_HANDLE_H
#define DNFSD_FILE_HANDLE_H

#include "file/file_base.h"

/**
 * @brief An NFSv3 handle
 *
 * This may be up to 64 bytes long, aligned on 32 bits
 */
typedef struct file_handle_v3 {
    uint8_t fhversion;	/*< Set to GANESHA_FH_VERSION */
    uint8_t fhflags1;	/*< To replace things like ds_flag */
    uint16_t exportid;	/*< Must be correlated to exportlist_t::id */
    uint8_t fs_len;		/*< Actual length of opaque handle */
    uint8_t fsopaque[];	/*< Persistent part of FSAL handle,
				    <= 59 bytes */
} file_handle_v3_t;

/**
 * @todo Danger Will Robinson!!
 * this struct is overlaid with fhandle3 in mount.h!!
 * This needs to be fixed.
 */
struct nfs_fh3 {
    struct {
        u_int data_len;
        char *data_val;
    } data;
};

#endif //DNFSD_FILE_HANDLE_H
