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

#ifndef DNFSD_DNFS_META_DATA_H
#define DNFSD_DNFS_META_DATA_H

#include "rpc/svc.h"
#include "nfs/nfsv41.h"
#include "nfs/nfs23.h"

struct dnfs_request_lookahead {
    uint32_t flags;
    uint16_t read;
    uint16_t write;
};

typedef struct nfs_core_param {
    /** The list of hosts allowed to use the HAProxy protocol. These are
     *  the hosts running HAProxy, acting as load balancing/proxy. Actual
     *  end clients are handled in EXPORT CLIENT lists.
     */
    struct glist_head haproxy_hosts;
    /** An array of port numbers, one for each protocol.  Set by
        the NFS_Port, MNT_Port, NLM_Port, and Rquota_Port options. */
    uint16_t port[P_COUNT];
    /** The IPv4 or IPv6 address to which to bind for our
        listening port.  Set by the Bind_Addr option. */
    sockaddr_t bind_addr;
    /** An array of RPC program numbers.  The correct values, by
        default, they may be set to incorrect values with the
        NFS_Program, MNT_Program, NLM_Program, and
        Rquota_Program.  It is debatable whether this is a
        worthwhile option to have. */
    uint32_t program[P_COUNT];
    /** For NFSv3, whether to drop rather than reply to requests
        yielding I/O errors.  True by default and settable with
        Drop_IO_Errors.  As this generally results in client
        retry, this seems like a dubious idea. */
    bool drop_io_errors;
    /** For NFSv3, whether to drop rather than reply to requests
        yielding invalid argument errors.  False by default and
        settable with Drop_Inval_Errors.  As this generally
        results in client retry, this seems like a really awful
        idea. */
    bool drop_inval_errors;
    /** For NFSv3, whether to drop rather than reply to requests
        yielding delay errors.  True by default and settable with
        Drop_Delay_Errors.  As this generally results in client
        retry and there is no NFSERR_DELAY, this seems like an
        excellent idea. */
    bool drop_delay_errors;
    /** Parameters controlling the Duplicate Request Cache.  */
    struct {
        /** Whether to disable the DRC entirely.  Defaults to
            false, settable by DRC_Disabled. */
        bool disabled;
        /* Parameters controlling TCP specific DRC behavior. */
        struct {
            /** Number of partitions in the tree for the
                TCP DRC.  Defaults to DRC_TCP_NPART,
                settable by DRC_TCP_Npart. */
            uint32_t npart;
            /** Maximum number of requests in a transport's
                DRC.  Defaults to DRC_TCP_SIZE and
                settable by DRC_TCP_Size. */
            uint32_t size;
            /** Number of entries in the O(1) front-end
                cache to a TCP Duplicate Request
                Cache.  Defaults to DRC_TCP_CACHESZ and
                settable by DRC_TCP_Cachesz. */
            uint32_t cachesz;
            /** High water mark for a TCP connection's
                DRC at which to start retiring entries if
                we can.  Defaults to DRC_TCP_HIWAT and
                settable by DRC_TCP_Hiwat. */
            uint32_t hiwat;
            /** Number of partitions in the recycle
                tree that holds per-connection DRCs so
                they can be used on reconnection (or
                recycled.)  Defaults to
                DRC_TCP_RECYCLE_NPART and settable by
                DRC_TCP_Recycle_Npart. */
            uint32_t recycle_npart;
            /** How long to wait (in seconds) before
                freeing the DRC of a disconnected
                client.  Defaults to
                DRC_TCP_RECYCLE_EXPIRE_S and settable by
                DRC_TCP_Recycle_Expire_S. */
            uint32_t recycle_expire_s;
            /** Whether to use a checksum to match
                requests as well as the XID.  Defaults to
                DRC_TCP_CHECKSUM and settable by
                DRC_TCP_Checksum. */
            bool checksum;
        } tcp;
        /** Parameters controlling UDP DRC behavior. */
        struct {
            /** Number of partitions in the tree for the
                UDP DRC.  Defaults to DRC_UDP_NPART,
                settable by DRC_UDP_Npart. */
            uint32_t npart;
            /** Maximum number of requests in the UDP DRC.
                Defaults to DRC_UDP_SIZE and settable by
                DRC_UDP_Size. */
            uint32_t size;
            /** Number of entries in the O(1) front-end
                cache to the UDP Duplicate Request
                Cache.  Defaults to DRC_UDP_CACHESZ and
                settable by DRC_UDP_Cachesz. */
            uint32_t cachesz;
            /** High water mark for the UDP DRC at which
                to start retiring entries if we can.
                Defaults to DRC_UDP_HIWAT and settable by
                DRC_UDP_Hiwat. */
            uint32_t hiwat;
            /** Whether to use a checksum to match
                requests as well as the XID.  Defaults to
                DRC_UDP_CHECKSUM and settable by
                DRC_UDP_Checksum. */
            bool checksum;
        } udp;
    } drc;
    /** Parameters affecting the relation with TIRPC.   */
    struct {
        /** Maximum number of connections for TIRPC.
            Defaults to 1024 and settable by
            RPC_Max_Connections. */
        uint32_t max_connections;
        /** Size of RPC send buffer.  Defaults to
            NFS_DEFAULT_SEND_BUFFER_SIZE and is settable by
            MaxRPCSendBufferSize.  */
        uint32_t max_send_buffer_size;
        /** Size of RPC receive buffer.  Defaults to
            NFS_DEFAULT_RECV_BUFFER_SIZE and is settable by
            MaxRPCRecvBufferSize. */
        uint32_t max_recv_buffer_size;
        /** Idle timeout (seconds).  Defaults to 5m */
        uint32_t idle_timeout_s;
        /** TIRPC ioq min simultaneous io threads.  Defaults to
            2 and settable by rpc_ioq_thrdmin. */
        uint32_t ioq_thrd_min;
        /** TIRPC ioq max simultaneous io threads.  Defaults to
            200 and settable by RPC_Ioq_ThrdMax. */
        uint32_t ioq_thrd_max;
        struct {
            /** Partitions in GSS ctx cache table (default 13). */
            uint32_t ctx_hash_partitions;
            /** Max GSS contexts in cache (i.e.,
             * max GSS clients, default 16K)
             */
            uint32_t max_ctx;
            /** Max entries to expire in one idle
             * check (default 200)
             */
            uint32_t max_gc;
        } gss;
    } rpc;
    /** Polling interval for blocked lock polling thread. */
    time_t blocked_lock_poller_interval;
    /** Protocols to support.  Should probably be renamed.
        Defaults to CORE_OPTION_ALL_VERS and is settable with
        NFS_Protocols (as a comma-separated list of 3 and 4.) */
    unsigned int core_options;
    /** Whether this Ganesha is part of a cluster of Ganeshas.
        This is somewhat vendor-specific and should probably be
        moved somewhere else.  Settable with Clustered. */
    bool clustered;
#ifdef _USE_NLM
    /** Whether to support the Network Lock Manager protocol.
	    Defaults to true and is settable with Enable_NLM. */
	bool enable_NLM;
	/** Whether to use the supplied name rather than the IP
	    address in NSM operations.  Settable with
	    NSM_Use_Caller_Name. */
	bool nsm_use_caller_name;
#endif
#ifdef _USE_RQUOTA
    /** Whether to support the Remote Quota protocol.  Defaults
	    to true and is settable with Enable_RQUOTA. */
	bool enable_RQUOTA;
#endif
#ifdef USE_NFSACL3
    /* Whether to support the POSIX ACL. Defaults to false. */
	bool enable_NFSACL;
#endif
    /** Whether to collect NFS stats.  Defaults to true. */
    bool enable_NFSSTATS;
    /** Whether to use fast stats.  Defaults to false. */
    bool enable_FASTSTATS;
    /** Whether to collect FSAL stats.  Defaults to false. */
    bool enable_FSALSTATS;
#ifdef _USE_NFS3
    /** Whether to collect NFSv3 Detailed stats.  Defaults to false. */
	bool enable_FULLV3STATS;
#endif
    /** Whether to collect NFSv4 Detailed stats.  Defaults to false. */
    bool enable_FULLV4STATS;
    /** Whether to collect Auth related stats. Defaults to false. */
    bool enable_AUTHSTATS;
    /** Whether to collect client all ops stats. Defaults to false. */
    bool enable_CLNTALLSTATS;
    /** Whether tcp sockets should use SO_KEEPALIVE */
    bool enable_tcp_keepalive;
    /** Maximum number of TCP probes before dropping the connection */
    uint32_t tcp_keepcnt;
    /** Idle time before TCP starts to send keepalive probes */
    uint32_t tcp_keepidle;
    /** Time between each keepalive probe */
    uint32_t tcp_keepintvl;
    /** Whether to use short NFS file handle to accommodate VMware
        NFS client. Enable this if you have a VMware NFSv3 client.
        VMware NFSv3 client has a max limit of 56 byte file handles!
        Defaults to false. */
    bool short_file_handle;
    /** How long the server will trust information it got by
        calling getgroups() when "Manage_Gids = TRUE" is
        used in a export entry. */
    time_t manage_gids_expiration;
    /** Path to the directory containing server specific
        modules.  In particular, this is where FSALs live. */
    char *ganesha_modules_loc;
    /** Frequency of dbus health heartbeat in ms. Set to 0 to disable */
    uint32_t heartbeat_freq;
    /** Whether to use device major/minor for fsid. Defaults to false. */
    bool fsid_device;
    /** How many times to attempt retry of stat while resolving POSIX
     *  filesystems */
    uint32_t resolve_fs_retries;
    /** Delay (in mili-seconds) between stat calls when trying to resolve
     *  POSIX filesystems */
    uint32_t resolve_fs_delay;
    /** Whether to use Pseudo (true) or Path (false) for NFS v3 and 9P
        mounts. */
    bool mount_path_pseudo;
    /** Whether to disable UDP listeners */
    uint32_t enable_UDP;
    /** DBus name prefix. Required if one wants to run multiple ganesha
        instances on single host. The prefix should be different for every
        ganesha instance. If this is set, dbus name will be
        <prefix>.org.ganesha.nfsd */
    char *dbus_name_prefix;
    /** Max parallel queries to Directory Server when Manage_Gids=True.
        Required if one does not want to overwhelm the directory server.
        The value limits the number of concurrent uid2grp requests.
        Useful when dealing with a slow Directory Service provider in an
        environment where users are part of large number of groups.
    */
    uint32_t max_uid_to_grp_reqs;
    /** Enable v3 filehandle to be used for v4 */
    bool enable_v3_fh_for_v4;
    /** Readdir response size, default is 64M (limited by maxcount from
    *  nfs request. range 4K-64M
    */
    uint32_t readdir_res_size;
    /** Readdir max entries count, default is 1M (limited by dircount from
    *  nfs request). range 32-1M
    */
    uint32_t readdir_max_count;
    /** Whether to call getattrs in nfs4_complete_read and
        nfs3_complete_read.
        Defaults to true and settable by Getattrs_In_Complete_Read. */
    bool getattrs_in_complete_read;
    /** Enable malloc trim */
    bool malloc_trim;
    /** Minimum threshold value to call malloc_trim. The malloc_trim
    * will be called once memory allocation exceeds minimum value.
    * Size in MB's. Note, this setting has no effect when
    * Enable_malloc_trim is set to false.
    */
    uint32_t malloc_trim_minthreshold;
    /** if  Manage_Gids=True and group resolution fails,
     *  then use gid data from rpc request */
    bool enable_rpc_cred_fallback;
} nfs_core_parameter_t;

typedef struct nfs_param {
    /* NFS的核心参数数据，主要是针对NFSV23协议的内容 */
    nfs_core_parameter_t core_param;
} nfs_parameter_t;

typedef enum protos {
    P_NFS,			/*< NFS, of course. */
    P_MNT,			/*< Mount (for v3) */
    P_COUNT			/*< Number of protocols */
} protos;

typedef union nfs_arg__ {
    GETATTR3args arg_getattr3;
    SETATTR3args arg_setattr3;
    LOOKUP3args arg_lookup3;
    ACCESS3args arg_access3;
    READLINK3args arg_readlink3;
    READ3args arg_read3;
    WRITE3args arg_write3;
    CREATE3args arg_create3;
    MKDIR3args arg_mkdir3;
    SYMLINK3args arg_symlink3;
    MKNOD3args arg_mknod3;
    REMOVE3args arg_remove3;
    RMDIR3args arg_rmdir3;
    RENAME3args arg_rename3;
    LINK3args arg_link3;
    READDIR3args arg_readdir3;
    READDIRPLUS3args arg_readdirplus3;
    FSSTAT3args arg_fsstat3;
    FSINFO3args arg_fsinfo3;
    PATHCONF3args arg_pathconf3;
    COMMIT3args arg_commit3;
    COMPOUND4args arg_compound4;

    /* mnt */
    mnt3_dirpath arg_mnt;

    /* nlm */
    nlm4_testargs arg_nlm4_test;
    nlm4_lockargs arg_nlm4_lock;
    nlm4_cancargs arg_nlm4_cancel;
    nlm4_shareargs arg_nlm4_share;
    nlm4_unlockargs arg_nlm4_unlock;
    nlm4_sm_notifyargs arg_nlm4_sm_notify;
    nlm4_free_allargs arg_nlm4_free_allargs;
    nlm4_res arg_nlm4_res;

    /* Rquota */
    getquota_args arg_rquota_getquota;
    getquota_args arg_rquota_getactivequota;
    setquota_args arg_rquota_setquota;
    setquota_args arg_rquota_setactivequota;

    /* Ext Rquota */
    ext_getquota_args arg_ext_rquota_getquota;
    ext_getquota_args arg_ext_rquota_getactivequota;
    ext_setquota_args arg_ext_rquota_setquota;
    ext_setquota_args arg_ext_rquota_setactivequota;

    /* NFSACL */
    getaclargs arg_getacl;
    setaclargs arg_setacl;
} nfs_arg_t;

typedef union nfs_res__ {
    GETATTR3res res_getattr3;
    SETATTR3res res_setattr3;
    LOOKUP3res res_lookup3;
    ACCESS3res res_access3;
    READLINK3res res_readlink3;
    READ3res res_read3;
    WRITE3res res_write3;
    CREATE3res res_create3;
    MKDIR3res res_mkdir3;
    SYMLINK3res res_symlink3;
    MKNOD3res res_mknod3;
    REMOVE3res res_remove3;
    RMDIR3res res_rmdir3;
    RENAME3res res_rename3;
    LINK3res res_link3;
    READDIR3res res_readdir3;
    READDIRPLUS3res res_readdirplus3;
    FSSTAT3res res_fsstat3;
    FSINFO3res res_fsinfo3;
    PATHCONF3res res_pathconf3;
    COMMIT3res res_commit3;
    struct COMPOUND4res_extended *res_compound4_extended;

    /* mount */
    fhstatus2 res_mnt1;
    mnt3_exports res_mntexport;
    mountres3 res_mnt3;
    mountlist res_dump;

    /* nlm4 */
    nlm4_testres res_nlm4test;
    nlm4_res res_nlm4;
    nlm4_shareres res_nlm4share;

    /* Rquota */
    getquota_rslt res_rquota_getquota;
    getquota_rslt res_rquota_getactivequota;
    setquota_rslt res_rquota_setquota;
    setquota_rslt res_rquota_setactivequota;

    /* Ext Rquota */
    getquota_rslt res_ext_rquota_getquota;
    getquota_rslt res_ext_rquota_getactivequota;
    setquota_rslt res_ext_rquota_setquota;
    setquota_rslt res_ext_rquota_setactivequota;

    /* NFSACL */
    getaclres res_getacl;
    setaclres res_setacl;
} nfs_res_t;

typedef int (*nfs_protocol_function_t) (nfs_arg_t *,
                                        struct svc_req *,
                                        nfs_res_t *);

typedef void (*nfs_protocol_free_t) (nfs_res_t *);

typedef struct nfs_function_desc__ {
    nfs_protocol_function_t service_function;
    nfs_protocol_free_t free_function;
    xdrproc_t xdr_decode_func;
    xdrproc_t xdr_encode_func;
    const char *funcname;
    unsigned int dispatch_behaviour;
} nfs_function_desc_t;

typedef struct nfs_request {
    struct svc_req svc;
    struct dnfs_request_lookahead lookahead;
//    nfs_arg_t arg_nfs;
//    nfs_res_t *res_nfs;
    const nfs_function_desc_t *funcdesc;
    void *proc_data;
    /** This request may be queued up pending completion of the request
     *  this is a dupreq of.
     */
    TAILQ_ENTRY(nfs_request) dupes;
} nfs_request_t;

#define NOTHING_SPECIAL 0x0000	/* Nothing to be done for this kind of
				   request */

#endif //DNFSD_DNFS_META_DATA_H
