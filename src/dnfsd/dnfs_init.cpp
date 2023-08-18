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
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>

#include <fstream>
#include <experimental/filesystem>

#include "rpc/svc.h"
#include "log/log.h"
#include "dnfsd/dnfs_init.h"
#include "dnfsd/dnfs_rpc_func.h"
#include "utils/common_utils.h"

using namespace std;

#define MODULE_NAME "main"

/* 默认配置文件的字符表达形式 */
static const char default_config[] =
        "# default config file content\n"
        "\n";

/* 创建一个默认初始化的配置 */
static void init_default_config(const string& config_file_path) {
    string config_dir = config_file_path.substr(
            config_file_path.find_last_of('/'));
    if (! experimental::filesystem::create_directories(config_dir)) {
        LOG(MODULE_NAME, EXIT_ERROR,
                   "Failed to create directory \"%s\" for config file.",
                   config_dir.c_str());
    }
    int config_fd = open(config_file_path.c_str(), O_WRONLY);
    if (config_fd == -1) {
        LOG(MODULE_NAME, EXIT_ERROR,
                   "Failed to create default config file \"%s\"",
                   config_file_path.c_str());
    }
    if (write(config_fd, default_config, strlen(default_config)) == -1) {
        LOG(MODULE_NAME, EXIT_ERROR,
                   "Failed to write default config to config file \"%s\"",
                   config_file_path.c_str());
    }
    close(config_fd);
}

/* 初始化配置文件并进行解析 */
void init_config(const string& config_file_path) {
    if (access(config_file_path.c_str(), F_OK) == -1) {
        /* 如果目标文件不存在，创建一个默认初始化的配置 */
        init_default_config(config_file_path);
    }
    dnfs_config = YAML::LoadFile(config_file_path);
    /* 这里还会做一些基本配置文件的校验 */
    /*TODO*/
}

/* 该函数用于快速从配置文件中获取指定层级的数据，并将结果按照指定格式写入到变量中，返回成功标志 */
template<typename T>
int config_get(T& out, const YAML::Node& config,
               const vector<string>& key_list) {
    int key_size = key_list.size();
    YAML::Node next_node = const_cast<YAML::Node&>(config);
    for (int i = 0; i < key_size; i++) {
        const string& key = key_list[i];
        YAML::NodeType::value type = config[key].Type();
        switch (type) {
            case YAML::NodeType::Map:
                if (i == key_size - 1) {
                    if (!(is_same<T, map<string, string>>::value or
                            is_same<T, map<string, char>>::value or
                            is_same<T, map<string, int>>::value or
                            is_same<T, map<string, long long>>::value or
                            is_same<T, map<string, float>>::value or
                            is_same<T, map<string, long double>>::value)) {
                        LOG(MODULE_NAME, L_WARN,
                                   "Config file node for %s cannot be"
                                   " interpreted as normal map class",
                                   format(key_list).c_str());
                        return 1;
                    } else {
                        out = next_node[key].as<T>();
                        return 0;
                    }
                }
                break;
            case YAML::NodeType::Sequence:
                if (i == key_size - 1) {
                    if (!(is_same<T, vector<string>>::value or
                            is_same<T, vector<char>>::value or
                            is_same<T, vector<int>>::value or
                            is_same<T, vector<long long>>::value or
                            is_same<T, vector<float>>::value or
                            is_same<T, vector<long double>>::value)) {
                        LOG(MODULE_NAME, L_WARN,
                                   "Config file node for %s cannot be"
                                   " interpreted as normal vector class",
                                   format(key_list).c_str());
                        return 1;
                    } else {
                        out = next_node[key].as<T>();
                        return 0;
                    }
                } else {
                    LOG(MODULE_NAME, L_WARN,
                               "List type config node can not be "
                               "indexed by \"%s(%d)\" in %s)",
                               key.c_str(), i, format(key_list).c_str());
                    return 1;
                }
            case YAML::NodeType::Scalar:
                if (i == key_size - 1) {
                    if (!(is_same<T, string>::value or
                          is_same<T, char>::value or
                          is_same<T, int>::value or
                          is_same<T, long long>::value or
                          is_same<T, float>::value or
                          is_same<T, long double>::value)) {
                        LOG(MODULE_NAME, L_WARN,
                                   "Config file node for %s cannot be"
                                   " interpreted as normal value type",
                                   format(key_list).c_str());
                        return 1;
                    } else {
                        out = next_node[key].as<T>();
                        return 0;
                    }
                } else {
                    LOG(MODULE_NAME, L_WARN,
                               "Value type config node can not be "
                               "indexed by \"%s(%d)\" in %s)",
                               key.c_str(), i, format(key_list).c_str());
                    return 1;
                }
            case YAML::NodeType::Null:
                LOG(MODULE_NAME, L_WARN,
                           "Unknown config node %s(%d) in %s",
                           key.c_str(), i, format(key_list).c_str());
                return 1;
            default:
                LOG(MODULE_NAME, L_WARN,
                           "Unknown config node type %d @%s(%d) in %s",
                           type, key.c_str(), i, format(key_list).c_str());
                return 1;
        }
        next_node = next_node[key];
    }
    /* Should never reach here */
    return 1;
}

/* 初始化日志相关的配置 */
void init_logging(const string& exec_name, const string& nfs_host_name,
                  const log_level_t debug_level, const bool detach_flag,
                  const string& arg_log_path) {
    /* 日志路径 */
    string log_path = arg_log_path;
    /*初始化日志管理器*/
    Logger::init(exec_name, nfs_host_name);
    /*初始化主程序日志*/
    logger.init_module(MODULE_NAME);
    /*初始化日志等级*/
    logger.set_log_level(debug_level);

    string temp;
    if (config_get(temp, dnfs_config, {"log", "path"})) {
        log_path = temp;
    }

    char buffer[256] = "";
    size_t buf_size = 0;
    /*初始化日志文件信息*/
    /* TODO 这里需要对参数进行校验避免buffer overflow */
    if (config_get(temp, dnfs_config, {"log", "limit_type"})) {
        buf_size += sprintf(buffer, "@(%s", temp.c_str());
        if (config_get(temp, dnfs_config, {"log", "limit_info"})) {
            buf_size += sprintf(buffer, ",%s", temp.c_str());
        }
        if (config_get(temp, dnfs_config, {"log", "backup_count"})) {
            buf_size += sprintf(buffer, ",%s", temp.c_str());
        }
        buffer[buf_size] = ')';
    }
    log_path += buffer;
    if (!detach_flag) {
        /* 只检查log_path部分的合法性 */
        if (logger.set_log_output(L_INFO, log_path + ":stdout:syslog", &temp)) {
            LOG(MODULE_NAME, EXIT_ERROR, temp);
        }
        logger.set_log_output({EXIT_EXCEPTION, EXIT_ERROR, L_ERROR, L_WARN},
                              log_path + ":stderr:syslog", nullptr);
        logger.set_log_output(D_INFO, log_path + ":stdout", nullptr);
        logger.set_log_output({D_ERROR, D_WARN}, log_path += ":stderr", nullptr);
    } else {
        /* 只检查log_path部分的合法性 */
        if (logger.set_log_output(log_path, &temp)) {
            LOG(MODULE_NAME, EXIT_ERROR, temp);
        }
        logger.set_log_output(L_INFO, log_path + ":syslog", nullptr);
        logger.set_log_output({EXIT_EXCEPTION, EXIT_ERROR, L_ERROR, L_WARN},
                              log_path + ":syslog", nullptr);
    }
    if (config_get(temp, dnfs_config, {"log", "formatter"})) {
        string error_info;
        if (logger.set_formatter(temp, &error_info)) {
            LOG(MODULE_NAME, EXIT_ERROR, error_info);
        }
    }
    Logger::set_default_attr_from(MODULE_NAME, nullptr);
}

/* 崩溃信号默认处理函数 */
static void crash_handler(int signo, [[maybe_unused]] siginfo_t *info,
                          [[maybe_unused]] void *ctx) {
    LOG("Crash Handler", L_BACKTRACE, "");
    /* re-raise the signal for the default signal handler to dump core */
    raise(signo);
}

/* 将处理函数挂载到对应的信号处理上 */
static void install_sighandler(int signo, void (*handler)(int, siginfo_t *, void *)) {
    struct sigaction sa = {};
    int ret;

    sa.sa_sigaction = handler;
    /* set SA_RESETHAND to restore default handler */
    /* SA_SIGINFO：使用更加详细的handler类型，传递三个参数而不是一个，获取更多的信息 */
    /* SA_RESETHAND：在执行过一次信号对应的处理函数后，重置handler为缺省的处理函数 */
    /* SA_NODEFER：执行处理函数的时候不会阻塞信号，即可以持续接收新的信号 */
    sa.sa_flags = SA_SIGINFO | SA_RESETHAND | SA_NODEFER;

    sigemptyset(&sa.sa_mask);

    /* sigaction函数用于改变进程接收到特定信号后的行为。该函数的第一个参数为信号的值，
     * 可以为除SIGKILL及SIGSTOP外的任何一个特定有效的信号（为这两个信号定义自己的处理函数，
     * 将导致信号安装错误）。第二个参数是指向结构sigaction的一个实例的指针，
     * 在结构sigaction的实例中，指定了对特定信号的处理，可以为空，进程会以缺省方式对信号处理；
     * 第三个参数oldact指向的对象用来保存原来对相应信号的处理，可指定oldact为NULL。*/
    ret = sigaction(signo, &sa, NULL);
    if (ret) {
        LOG(MODULE_NAME, L_WARN,
                "Install handler for signal (%s) failed",
                strsignal(signo));
    }
}

/* 初始化错误信号的处理函数 */
void init_crash_handlers(void) {
    install_sighandler(SIGSEGV, crash_handler);
    install_sighandler(SIGABRT, crash_handler);
    install_sighandler(SIGBUS, crash_handler);
    install_sighandler(SIGILL, crash_handler);
    install_sighandler(SIGFPE, crash_handler);
    install_sighandler(SIGQUIT, crash_handler);
}

/* 检查malloc功能的可用性 */
void init_check_malloc() {
    /* Check malloc(0) - DNFS assumes malloc(0) returns non-NULL pointer.
     * Note we use malloc and calloc directly here and not gsh_malloc and
     * gsh_calloc because we don't want those functions to abort(), we
     * want to log a descriptive message.
     */
    void *p;

    p = malloc(0);
    if (p == NULL) {
        LOG(MODULE_NAME, EXIT_ERROR,
                 "DNFS's assumption that malloc(0) returns a non-NULL pointer"
                 " is not true, Ganesha can not work with the memory allocator in use. Aborting.");
    }
    free(p);

    p = calloc(0, 0);
    if (p == NULL) {
        LOG(MODULE_NAME, EXIT_ERROR,
                 "Ganesha's assumption that calloc(0, 0) returns a non-NULL pointer"
                 " is not true, Ganesha can not work with the memory allocator in use. Aborting.");
    }
    free(p);
}

/* 初始化线程对信号的处理操作 */
int init_thread_signal_mask() {
    /* 解除文件大小资源限制，对相关的停止信号进行忽略 */
    signal(SIGXFSZ, SIG_IGN);
    /* 在所有默认子线程中阻塞指定的信号以便专门的信号处理线程能够正确处理信号 */
    sigset_t signals_to_block;
    sigemptyset(&signals_to_block);
    /* SIGTERM 终止进程 软件终止信号 */
    sigaddset(&signals_to_block, SIGTERM);
    /* SIGHUP 终止进程 终端线路挂断 */
    sigaddset(&signals_to_block, SIGHUP);
    /* SIGPIPE 终止进程 向一个没有读进程的管道写数据 */
    sigaddset(&signals_to_block, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &signals_to_block, NULL) != 0) {
        LOG(MODULE_NAME, EXIT_ERROR,
                 "Could not start nfs daemon, pthread_sigmask failed");
        return -1;
    }
    return 0;
}

/* 用于打印当前系统使用的配置信息 */
static void dump_config() {
    /*TODO*/
}

/* 初始化nfs服务相关的接口注册操作 */
static void dnfs_init_svc(void)
{
    svc_init_params svc_params;
    int ix;
    int code;

    LOG(MODULE_NAME, L_INFO, "DNFS INIT: using TIRPC");

    memset(&svc_params, 0, sizeof(svc_params));

    /* New TI-RPC package init function */
    svc_params.disconnect_cb = NULL;
    svc_params.alloc_cb = alloc_nfs_request;
    svc_params.free_cb = free_nfs_request;
    svc_params.flags = SVC_INIT_EPOLL;	/* use EPOLL event mgmt */
    svc_params.flags |= SVC_INIT_NOREG_XPRTS; /* don't call xprt_register */
    svc_params.max_connections = nfs_param.core_param.rpc.max_connections;
    svc_params.max_events = 1024;	/* length of epoll event queue */
    svc_params.ioq_send_max =
            nfs_param.core_param.rpc.max_send_buffer_size;
    svc_params.channels = N_EVENT_CHAN;
    svc_params.idle_timeout = nfs_param.core_param.rpc.idle_timeout_s;
    svc_params.ioq_thrd_min = nfs_param.core_param.rpc.ioq_thrd_min;
    svc_params.ioq_thrd_max = nfs_param.core_param.rpc.ioq_thrd_max;
    /* GSS ctx cache tuning, expiration */
    svc_params.gss_ctx_hash_partitions =
            nfs_param.core_param.rpc.gss.ctx_hash_partitions;
    svc_params.gss_max_ctx =
            nfs_param.core_param.rpc.gss.max_ctx;
    svc_params.gss_max_gc =
            nfs_param.core_param.rpc.gss.max_gc;

    /* Only after TI-RPC allocators, log channel are setup */
    if (!svc_init(&svc_params))
        LogFatal(COMPONENT_INIT, "SVC initialization failed");

    for (ix = 0; ix < EVCHAN_SIZE; ++ix) {
        rpc_evchan[ix].chan_id = 0;
        code = svc_rqst_new_evchan(&rpc_evchan[ix].chan_id,
                                   NULL /* u_data */,
                                   SVC_RQST_FLAG_NONE);
        if (code)
            LogFatal(COMPONENT_DISPATCH,
                     "Cannot create TI-RPC event channel (%d, %d)",
                     ix, code);
        /* XXX bail?? */
    }

    /* Allocate the UDP and TCP sockets for the RPC */
    Allocate_sockets();

    if ((NFS_options & CORE_OPTION_ALL_NFS_VERS) != 0) {
        /* Bind the tcp and udp sockets */
        Bind_sockets();

        /* Unregister from portmapper/rpcbind */
        unregister_rpc();

        /* Set up well-known xprt handles */
        Create_SVCXPRTs();
    }

    /*
	 * Perform all the RPC registration, for UDP and TCP, on both NFS_V3
	 * and NFS_V4. Note that v4 servers are not required to register with
	 * rpcbind, so we don't fail to start if only that fails.
	 */
#ifdef _USE_NFS3
	if (NFS_options & CORE_OPTION_NFSV3) {
		Register_program(P_NFS, NFS_V3);
		Register_program(P_MNT, MOUNT_V1);
		Register_program(P_MNT, MOUNT_V3);
#ifdef _USE_NLM
		if (nfs_param.core_param.enable_NLM)
			Register_program(P_NLM, NLM4_VERS);
#endif /* _USE_NLM */
#ifdef USE_NFSACL3
		if (nfs_param.core_param.enable_NFSACL)
			Register_program(P_NFSACL, NFSACL_V3);
#endif
	}
#endif /* _USE_NFS3 */

	/* v4 registration is optional */
	if (NFS_options & CORE_OPTION_NFSV4)
		__Register_program(P_NFS, NFS_V4);

#ifdef _USE_RQUOTA
	if (nfs_param.core_param.enable_RQUOTA &&
	    (NFS_options & CORE_OPTION_ALL_NFS_VERS)) {
		Register_program(P_RQUOTA, RQUOTAVERS);
		Register_program(P_RQUOTA, EXT_RQUOTAVERS);
	}
#endif
}

void nfs_Init_admin_thread(void) {
    PTHREAD_MUTEX_init(&admin_control_mtx, NULL);
    PTHREAD_COND_init(&admin_control_cv, NULL);
#ifdef USE_DBUS
    gsh_dbus_register_path("admin", admin_interfaces);
#endif                /* USE_DBUS */
    LogEvent(COMPONENT_NFS_CB, "Admin thread initialized");
}

/**
 * @brief Init the nfs daemon
 *
 * @param[in] p_start_info Unused
 */
static void dnfs_init(const nfs_start_info_t *nfs_start_info) {
    /* RPC Initialisation - exits on failure */
    dnfs_init_svc();
    LogInfo(COMPONENT_INIT, "RPC resources successfully initialized");

    /* Admin initialisation */
    nfs_Init_admin_thread();
}

/* dnfs启动处理函数 */
void dnfs_start(nfs_start_info_t *nfs_start_info) {
    if (nfs_start_info->dump_default_config == true) {
        dump_config();
        exit(0);
    }

    /* Make sure DNFS runs with a 0000 umask. */
    umask(0000);

    /* Initialize all layers and service threads */
    dnfs_init(nfs_start_info);
    nfs_Start_threads(); /* Spawns service threads */

    nfs_init_complete();

    LogEvent(COMPONENT_INIT,
             "-------------------------------------------------");
    LogEvent(COMPONENT_INIT, "             NFS SERVER INITIALIZED");
    LogEvent(COMPONENT_INIT,
             "-------------------------------------------------");

    /* Wait for dispatcher to exit */
    LogDebug(COMPONENT_THREAD, "Wait for admin thread to exit");
    pthread_join(admin_thrid, NULL);

    /* Regular exit */
    LogEvent(COMPONENT_MAIN, "NFS EXIT: regular exit");

    nfs_init_cleanup();

    Cleanup();
    /* let main return 0 to exit */
}
