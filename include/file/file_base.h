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

#ifndef DNFSD_FILE_BASE_H
#define DNFSD_FILE_BASE_H

#include <stdint.h>
#include <sys/types.h>

/**
 * @brief Object file type within the system
 */
typedef enum {
    NO_FILE_TYPE = 0,	/* sanity check to ignore type */
    REGULAR_FILE = 1,
    CHARACTER_FILE = 2,
    BLOCK_FILE = 3,
    SYMBOLIC_LINK = 4,
    SOCKET_FILE = 5,
    FIFO_FILE = 6,
    DIRECTORY = 7,
    EXTENDED_ATTR = 8
} object_file_type_t;

/**
 * Defines an attribute mask.
 *
 * Do not just use OR and AND to test these, use the macros.
 */
typedef uint64_t attrmask_t;

typedef uint32_t fsal_acetype_t;

typedef uint32_t fsal_aceperm_t;

typedef uint32_t fsal_aceflag_t;

/** filesystem identifier */
typedef struct fsal_fsid__ {
    uint64_t major;
    uint64_t minor;
} fsal_fsid_t;

typedef struct {
    u_int utf8string_len;
    char *utf8string_val;
} utf8string;

typedef struct fsal_ace__ {
    fsal_acetype_t type;
    fsal_aceperm_t perm;

    fsal_aceflag_t flag;
    fsal_aceflag_t iflag;	/* Internal flags. */
    union {
        uid_t uid;
        gid_t gid;
    } who;
} fsal_ace_t;

typedef struct fsal_acl__ {
    uint32_t naces;
    fsal_ace_t *aces;
    pthread_rwlock_t acl_lock;
    uint32_t ref;
} fsal_acl_t;

/**
 * Stores root and fs locations. fs locations format is as follows
 *
 * <server>:<path>
 */
typedef struct fsal_fs_locations {
    uint32_t ref;
    uint32_t nservers;	/* size of server array */
    pthread_rwlock_t fsloc_lock;
    char *fs_root;
    char *rootpath;
    utf8string *server;
} fsal_fs_locations_t;

/** raw device spec */
typedef struct fsal_dev__ {
    uint64_t major;
    uint64_t minor;
} fsal_dev_t;

/**
 * @brief A list of FS object's attributes.
 */
struct fsal_attrlist {
    attrmask_t request_mask; /*< Indicates the requested from the FSAL. */
    attrmask_t valid_mask;	/*< Indicates the attributes to be set or
				   that have been filled in by the FSAL. */
    attrmask_t supported;	/*< Indicates which attributes the FSAL
				    supports. */
    object_file_type_t type;	/*< Type of this object */
    uint64_t filesize;	/*< Logical size (amount of data that can be
				   read) */
    fsal_fsid_t fsid;	/*< Filesystem on which this object is
				   stored */
    uint64_t fsid3;		/*< Squashed fsid for NFS v3 */
    fsal_acl_t *acl;	/*< ACL for this object */
    uint64_t fileid;	/*< Unique identifier for this object within
				   the scope of the fsid, (e.g. inode number) */
    uint32_t mode;		/*< POSIX access mode */
    uint32_t numlinks;	/*< Number of links to this file */
    uint64_t owner;		/*< Owner ID */
    uint64_t group;		/*< Group ID */
    fsal_dev_t rawdev;	/*< Major/minor device number (only
				   meaningful for character/block special
				   files.) */
    struct timespec atime;	/*< Time of last access */
    struct timespec creation;	/*< Creation time */
    struct timespec ctime;	/*< Inode modification time (a la stat.
				   NOT creation.) */
    struct timespec mtime;	/*< Time of last modification */
    uint64_t spaceused;	/*< Space used on underlying filesystem */
    uint64_t change;	/*< A 'change id' */
    uint64_t generation;	/*< Generation number for this file */
    int32_t expire_time_attr;	/*< Expiration time interval in seconds
					   for attributes. Settable by FSAL. */
    fsal_fs_locations_t *fs_locations;	/*< fs locations for this
						    object if any */
//    struct sec_label4 sec_label;
};

/**
 * @brief File system dynamic info.
 */
typedef struct fsal_dynamicfsinfo__ {
    uint64_t total_bytes;
    uint64_t free_bytes;
    uint64_t avail_bytes;
    uint64_t total_files;
    uint64_t free_files;
    uint64_t avail_files;
    uint64_t maxread;
    uint64_t maxwrite;
    struct timespec time_delta;
} fsal_dynamicfsinfo_t;

/**
 * @brief The return error values of FSAL calls.
 */
typedef enum fsal_errors_t {
    ERR_FSAL_NO_ERROR = 0,
    ERR_FSAL_PERM = 1,
    ERR_FSAL_NOENT = 2,
    ERR_FSAL_IO = 5,
    ERR_FSAL_NXIO = 6,
    ERR_FSAL_NOMEM = 12,
    ERR_FSAL_ACCESS = 13,
    ERR_FSAL_FAULT = 14,
    ERR_FSAL_STILL_IN_USE = 16,
    ERR_FSAL_EXIST = 17,
    ERR_FSAL_XDEV = 18,
    ERR_FSAL_NOTDIR = 20,
    ERR_FSAL_ISDIR = 21,
    ERR_FSAL_INVAL = 22,
    ERR_FSAL_FBIG = 27,
    ERR_FSAL_NOSPC = 28,
    ERR_FSAL_ROFS = 30,
    ERR_FSAL_MLINK = 31,
    ERR_FSAL_DQUOT = 49,
    ERR_FSAL_NO_DATA = 61,
    ERR_FSAL_NAMETOOLONG = 78,
    ERR_FSAL_NOTEMPTY = 93,
    ERR_FSAL_STALE = 151,
    ERR_FSAL_BADHANDLE = 10001,
    ERR_FSAL_BADCOOKIE = 10003,
    ERR_FSAL_NOTSUPP = 10004,
    ERR_FSAL_TOOSMALL = 10005,
    ERR_FSAL_SERVERFAULT = 10006,
    ERR_FSAL_BADTYPE = 10007,
    ERR_FSAL_DELAY = 10008,
    ERR_FSAL_LOCKED = 10012,
    ERR_FSAL_FHEXPIRED = 10014,
    ERR_FSAL_SHARE_DENIED = 10015,
    ERR_FSAL_SYMLINK = 10029,
    ERR_FSAL_ATTRNOTSUPP = 10032,
    ERR_FSAL_BAD_RANGE = 10042,
    ERR_FSAL_NOT_INIT = 20001,
    ERR_FSAL_ALREADY_INIT = 20002,
    ERR_FSAL_BAD_INIT = 20003,
    ERR_FSAL_SEC = 20004,
    ERR_FSAL_NO_QUOTA = 20005,
    ERR_FSAL_NOT_OPENED = 20010,
    ERR_FSAL_DEADLOCK = 20011,
    ERR_FSAL_OVERFLOW = 20012,
    ERR_FSAL_INTERRUPT = 20013,
    ERR_FSAL_BLOCKED = 20014,
    ERR_FSAL_TIMEOUT = 20015,
    ERR_FSAL_FILE_OPEN = 10046,
    ERR_FSAL_UNION_NOTSUPP = 10090,
    ERR_FSAL_IN_GRACE = 10095,
    ERR_FSAL_NO_ACE = 10096,
    ERR_FSAL_CROSS_JUNCTION = 10097,
    ERR_FSAL_BADNAME = 10098,
    ERR_FSAL_NOXATTR = 10099,
    ERR_FSAL_XATTR2BIG = 10100,
} fsal_errors_t;

/**
 * @brief The return status of FSAL calls.
 */
typedef struct fsal_status__ {
    fsal_errors_t major;	/*< FSAL status code */
    int minor;		/*< Other error code (usually POSIX) */
} fsal_status_t;

#endif //DNFSD_FILE_BASE_H
