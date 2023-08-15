
#include "dnfsd/dnfsd.h"
#include "dnfsd/dnfs_init.h"
#include "dnfsd/dnfs_dispatcher.h"
#include "dnfsd/dnfs_admin.h"
#include "common_utils.h"
#include "nfs/nfsv41.h"
#include "nfs/nfs23.h"

static struct _nfs_health healthstats;
struct _nfs_health nfs_health_;
struct nfs_init nfs_init;
nfs_start_info_t nfs_start_info;

nfs_parameter_t nfs_param;

pthread_mutexattr_t default_mutex_attr;
pthread_rwlockattr_t default_rwlock_attr;

writeverf3 NFS3_write_verifier;    /*< NFS V3 write verifier */
time_t nfs_ServerEpoch;

pthread_t admin_thrid;
pthread_t sigmgr_thrid;

tirpc_pkg_params ntirpc_pp = {
        TIRPC_DEBUG_FLAG_DEFAULT,
        0,
        SetNameFunction,
        (mem_format_t) rpc_warnx,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
};

static void install_sighandler(int signo,
                               void (*handler)(int, siginfo_t *, void *)) {
    struct sigaction sa = {};
    int ret;

    sa.sa_sigaction = handler;
    /* set SA_RESETHAND to restore default handler */
    sa.sa_flags = SA_SIGINFO | SA_RESETHAND | SA_NODEFER;

    sigemptyset(&sa.sa_mask);

    ret = sigaction(signo, &sa, NULL);
    if (ret) {
        LogWarn(COMPONENT_INIT,
                "Install handler for signal (%s) failed",
                strsignal(signo));
    }
}

static void crash_handler(int signo, siginfo_t *info, void *ctx) {
//    gsh_backtrace();
    /* re-raise the signal for the default signal handler to dump core */
    raise(signo);
}

static void init_crash_handlers(void) {
    install_sighandler(SIGSEGV, crash_handler);
    install_sighandler(SIGABRT, crash_handler);
    install_sighandler(SIGBUS, crash_handler);
    install_sighandler(SIGILL, crash_handler);
    install_sighandler(SIGFPE, crash_handler);
    install_sighandler(SIGQUIT, crash_handler);
}

void nfs_init_init(void) {
    PTHREAD_MUTEX_init(&nfs_init.init_mutex, NULL);
    PTHREAD_COND_init(&nfs_init.init_cond, NULL);
    nfs_init.init_complete = false;
}

/**
 * check for usable malloc implementation
 */
inline void nfs_check_malloc(void) {
    /* Check malloc(0) - Ganesha assumes malloc(0) returns non-NULL pointer.
     * Note we use malloc and calloc directly here and not gsh_malloc and
     * gsh_calloc because we don't want those functions to abort(), we
     * want to log a descriptive message.
     */
    void *p;

    p = malloc(0);
    if (p == NULL)
        LogFatal(COMPONENT_MAIN,
                 "Ganesha's assumption that malloc(0) returns a non-NULL pointer is not true, Ganesha can not work with the memory allocator in use. Aborting.");
    free(p);
    p = calloc(0, 0);
    if (p == NULL)
        LogFatal(COMPONENT_MAIN,
                 "Ganesha's assumption that calloc(0, 0) returns a non-NULL pointer is not true, Ganesha can not work with the memory allocator in use. Aborting.");
    free(p);
}

/**
 * @brief Init the nfs daemon
 *
 * @param[in] p_start_info Unused
 */

static void nfs_Init(const nfs_start_info_t *p_start_info) {
    /* RPC Initialisation - exits on failure */
    nfs_Init_svc();
    LogInfo(COMPONENT_INIT, "RPC resources successfully initialized");

    /* Admin initialisation */
    nfs_Init_admin_thread();
}

/**
 * @brief This thread is in charge of signal management
 *
 * @param[in] UnusedArg Unused
 *
 * @return NULL.
 */
static void *sigmgr_thread(void *UnusedArg) {
    int signal_caught = 0;

    SetNameFunction("sigmgr");

    /* Loop until we catch SIGTERM */
    while (signal_caught != SIGTERM) {
        sigset_t signals_to_catch;

        sigemptyset(&signals_to_catch);
        sigaddset(&signals_to_catch, SIGTERM);
        sigaddset(&signals_to_catch, SIGHUP);
        if (sigwait(&signals_to_catch, &signal_caught) != 0) {
            LogFullDebug(COMPONENT_THREAD,
                         "sigwait exited with error");
            continue;
        }
        if (signal_caught == SIGHUP) {
            LogEvent(COMPONENT_MAIN,
                     "SIGHUP_HANDLER: Received SIGHUP.... initiating export list reload");
        }
    }
    LogDebug(COMPONENT_THREAD, "sigmgr thread exiting");

    admin_halt();

    return NULL;
}

static void nfs_Start_threads(void) {
    int rc = 0;
    pthread_attr_t attr_thr;

    LogDebug(COMPONENT_THREAD, "Starting threads");

    /* Init for thread parameter (mostly for scheduling) */
    PTHREAD_ATTR_init(&attr_thr);
    PTHREAD_ATTR_setscope(&attr_thr, PTHREAD_SCOPE_SYSTEM);
    PTHREAD_ATTR_setdetachstate(&attr_thr, PTHREAD_CREATE_JOINABLE);

    /* Starting the thread dedicated to signal handling */
    rc = pthread_create(&sigmgr_thrid, &attr_thr, sigmgr_thread, NULL);
    if (rc != 0) {
        LogFatal(COMPONENT_THREAD,
                 "Could not create sigmgr_thread, error = %d (%s)",
                 errno, strerror(errno));
    }
    LogDebug(COMPONENT_THREAD, "sigmgr thread started");

    /* Starting the admin thread */
    rc = pthread_create(&admin_thrid, &attr_thr, admin_thread, NULL);
    if (rc != 0) {
        LogFatal(COMPONENT_THREAD,
                 "Could not create admin_thread, error = %d (%s)",
                 errno, strerror(errno));
    }
    LogEvent(COMPONENT_THREAD, "admin thread was started successfully");

    PTHREAD_ATTR_destroy(&attr_thr);
}

void nfs_init_complete(void) {
    PTHREAD_MUTEX_lock(&nfs_init.init_mutex);
    nfs_init.init_complete = true;
    pthread_cond_broadcast(&nfs_init.init_cond);
    PTHREAD_MUTEX_unlock(&nfs_init.init_mutex);
}

void nfs_init_cleanup(void) {
    PTHREAD_MUTEX_destroy(&nfs_init.init_mutex);
    PTHREAD_COND_destroy(&nfs_init.init_cond);
}

/**
 * @brief Start NFS service
 *
 * @param[in] p_start_info Startup parameters
 */
void nfs_start(nfs_start_info_t *p_start_info) {
    /* store the start info so it is available for all layers */
    nfs_start_info = *p_start_info;

    if (p_start_info->dump_default_config == true) {
        exit(0);
    }

    /* Make sure Ganesha runs with a 0000 umask. */
    umask(0000);

    {
        /* Set the write verifiers */
        union {
            writeverf3 NFS3_write_verifier;
            uint64_t epoch;
        } build_verifier;

        build_verifier.epoch = (uint64_t) nfs_ServerEpoch;

        memcpy(NFS3_write_verifier, build_verifier.NFS3_write_verifier,
               sizeof(NFS3_write_verifier));
    }

    /* Initialize all layers and service threads */
    nfs_Init(p_start_info);
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