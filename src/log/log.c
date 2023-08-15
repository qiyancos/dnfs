//
// Created by iecas on 2023/8/11.
//

/*
 * Convert a numeral log level in ascii to
 * the numeral value.
 */
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "log/log.h"
#include "log/display.h"
#include "utils/common_utils.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

/* Original log level set by -N or otherwise code default */
log_levels_t original_log_level = NIV_EVENT;

/**
 * @brief Default logging levels
 *
 * These are for early initialization and whenever we
 * have to fall back to something that will at least work...
 */

static log_levels_t default_log_levels[] = {
        [COMPONENT_ALL] = NIV_NULL,
        [COMPONENT_LOG] = NIV_EVENT,
        [COMPONENT_MEM_ALLOC] = NIV_EVENT,
        [COMPONENT_MEMLEAKS] = NIV_EVENT,
        [COMPONENT_FSAL] = NIV_EVENT,
        [COMPONENT_NFSPROTO] = NIV_EVENT,
        [COMPONENT_NFS_V4] = NIV_EVENT,
        [COMPONENT_EXPORT] = NIV_EVENT,
        [COMPONENT_FILEHANDLE] = NIV_EVENT,
        [COMPONENT_DISPATCH] = NIV_EVENT,
        [COMPONENT_MDCACHE] = NIV_EVENT,
        [COMPONENT_MDCACHE_LRU] = NIV_EVENT,
        [COMPONENT_HASHTABLE] = NIV_EVENT,
        [COMPONENT_HASHTABLE_CACHE] = NIV_EVENT,
        [COMPONENT_DUPREQ] = NIV_EVENT,
        [COMPONENT_INIT] = NIV_EVENT,
        [COMPONENT_MAIN] = NIV_EVENT,
        [COMPONENT_IDMAPPER] = NIV_EVENT,
        [COMPONENT_NFS_READDIR] = NIV_EVENT,
        [COMPONENT_NFS_V4_LOCK] = NIV_EVENT,
        [COMPONENT_CONFIG] = NIV_EVENT,
        [COMPONENT_CLIENTID] = NIV_EVENT,
        [COMPONENT_SESSIONS] = NIV_EVENT,
        [COMPONENT_PNFS] = NIV_EVENT,
        [COMPONENT_RW_LOCK] = NIV_EVENT,
        [COMPONENT_NLM] = NIV_EVENT,
        [COMPONENT_RPC] = NIV_EVENT,
        [COMPONENT_TIRPC] = NIV_EVENT,
        [COMPONENT_NFS_CB] = NIV_EVENT,
        [COMPONENT_THREAD] = NIV_EVENT,
        [COMPONENT_NFS_V4_ACL] = NIV_EVENT,
        [COMPONENT_STATE] = NIV_EVENT,
        [COMPONENT_9P] = NIV_EVENT,
        [COMPONENT_9P_DISPATCH] = NIV_EVENT,
        [COMPONENT_FSAL_UP] = NIV_EVENT,
        [COMPONENT_DBUS] = NIV_EVENT,
        [COMPONENT_NFS_MSK] = NIV_EVENT,
};

/* Active set of log levels */
log_levels_t *component_log_level = default_log_levels;

/**
 * @brief Define a set of possible time and date formats.
 *
 * These values will be stored in lf_ext for the LF_DATE and LF_TIME flags.
 *
 */
enum timedate_formats_t {
    TD_NONE,		/*< No time/date. */
    TD_GANESHA,		/*< Legacy Ganesha time and date format. */
    TD_LOCAL,		/*< Use strftime local format for time/date. */
    TD_8601,		/*< Use ISO 8601 time/date format. */
    TD_SYSLOG,		/*< Use a typical syslog time/date format. */
    TD_SYSLOG_USEC,		/*< Use a typical syslog time/date format that
				    also includes microseconds. */
    TD_USER,		/* Use a user defined time/date format. */
};

/**
 * @brief Format control for log messages
 *
 */

struct logfields {
    bool disp_epoch;
    bool disp_clientip;
    bool disp_host;
    bool disp_prog;
    bool disp_pid;
    bool disp_threadname;
    bool disp_filename;
    bool disp_linenum;
    bool disp_funct;
    bool disp_comp;
    bool disp_level;
    bool disp_op_id;
    enum timedate_formats_t datefmt;
    enum timedate_formats_t timefmt;
    char *user_date_fmt;
    char *user_time_fmt;
};

/**
 * @brief Startup default log message format
 *
 * Baked in here so early startup has something to work with
 */

static struct logfields default_logfields = {
        .disp_epoch = true,
        .disp_host = true,
        .disp_prog = true,
        .disp_pid = true,
        .disp_threadname = true,
        .disp_filename = false,
        .disp_linenum = false,
        .disp_funct = true,
        .disp_comp = true,
        .disp_level = true,
        .datefmt = TD_GANESHA,
        .timefmt = TD_GANESHA
};

static struct logfields *logfields = &default_logfields;

/* Define the maximum length of a user time/date format. */
#define MAX_TD_USER_LEN 64
/* Define the maximum overall time/date format length, should have room
 * for both user date and user time format plus room for blanks around them.
 */
#define MAX_TD_FMT_LEN (MAX_TD_USER_LEN * 2 + 4)

char const_log_str[LOG_BUFF_LEN] = "\0";
char date_time_fmt[MAX_TD_FMT_LEN] = "\0";

typedef struct loglev {
    char *str;
    char *short_str;
    int syslog_level;
} log_level_t;

static log_level_t tabLogLevel[] = {
        [NIV_NULL] = {"NIV_NULL", "NULL", LOG_NOTICE},
        [NIV_FATAL] = {"NIV_FATAL", "FATAL", LOG_CRIT},
        [NIV_MAJ] = {"NIV_MAJ", "MAJ", LOG_CRIT},
        [NIV_CRIT] = {"NIV_CRIT", "CRIT", LOG_ERR},
        [NIV_WARN] = {"NIV_WARN", "WARN", LOG_WARNING},
        [NIV_EVENT] = {"NIV_EVENT", "EVENT", LOG_NOTICE},
        [NIV_INFO] = {"NIV_INFO", "INFO", LOG_INFO},
        [NIV_DEBUG] = {"NIV_DEBUG", "DEBUG", LOG_DEBUG},
        [NIV_MID_DEBUG] = {"NIV_MID_DEBUG", "M_DBG", LOG_DEBUG},
        [NIV_FULL_DEBUG] = {"NIV_FULL_DEBUG", "F_DBG", LOG_DEBUG}
};

/**
 * @brief Format a string into the buffer.
 *
 * @param[in,out] dspbuf The buffer.
 * @param[in]     fmt    the format string
 * @param[in] ... the    args
 *
 * @return the bytes remaining in the buffer.
 *
 */
static inline int display_printf(struct display_buffer *dspbuf, const char *fmt,
                                 ...)
{
    va_list args;
    int b_left;

    va_start(args, fmt);

    b_left = display_vprintf(dspbuf, fmt, args);

    va_end(args);

    return b_left;
}

extern time_t nfs_ServerEpoch;
static char program_name[1024];
static char hostname[256];

void set_const_log_str(void)
{
    struct display_buffer dspbuf = { sizeof(const_log_str),
                                     const_log_str,
                                     const_log_str
    };
    struct display_buffer tdfbuf = { sizeof(date_time_fmt),
                                     date_time_fmt,
                                     date_time_fmt
    };
    int b_left = display_start(&dspbuf);

    const_log_str[0] = '\0';

    if (b_left > 0 && logfields->disp_epoch)
        b_left = display_printf(&dspbuf,
                                ": epoch %08lx ", (unsigned long) nfs_ServerEpoch);

    if (b_left > 0 && logfields->disp_host)
        b_left = display_printf(&dspbuf, ": %s ", hostname);

    if (b_left > 0 && logfields->disp_prog)
        b_left = display_printf(&dspbuf, ": %s", program_name);

    if (b_left > 0 && logfields->disp_prog
        && logfields->disp_pid)
        b_left = display_cat(&dspbuf, "-");

    if (b_left > 0 && logfields->disp_pid)
        b_left = display_printf(&dspbuf, "%d", getpid());

    if (b_left > 0
        && (logfields->disp_prog || logfields->disp_pid)
        && !logfields->disp_threadname)
        (void) display_cat(&dspbuf, " ");

    b_left = display_start(&tdfbuf);

    if (b_left <= 0)
        return;

    if (logfields->datefmt == TD_LOCAL
        && logfields->timefmt == TD_LOCAL) {
        b_left = display_cat(&tdfbuf, "%c ");
    } else {
        switch (logfields->datefmt) {
            case TD_GANESHA:
                b_left = display_cat(&tdfbuf, "%d/%m/%Y ");
                break;
            case TD_8601:
                b_left = display_cat(&tdfbuf, "%F ");
                break;
            case TD_LOCAL:
                b_left = display_cat(&tdfbuf, "%x ");
                break;
            case TD_SYSLOG:
                b_left = display_cat(&tdfbuf, "%b %e ");
                break;
            case TD_SYSLOG_USEC:
                if (logfields->timefmt == TD_SYSLOG_USEC)
                    b_left = display_cat(&tdfbuf, "%F");
                else
                    b_left = display_cat(&tdfbuf, "%F ");
                break;
            case TD_USER:
                b_left = display_printf(&tdfbuf, "%s ",
                                        logfields->user_date_fmt);
                break;
            case TD_NONE:
            default:
                break;
        }

        if (b_left <= 0)
            return;

        switch (logfields->timefmt) {
            case TD_GANESHA:
                b_left = display_cat(&tdfbuf, "%H:%M:%S ");
                break;
            case TD_SYSLOG:
            case TD_8601:
            case TD_LOCAL:
                b_left = display_cat(&tdfbuf, "%X ");
                break;
            case TD_SYSLOG_USEC:
                b_left = display_cat(&tdfbuf, "T%H:%M:%S.%%06u%z ");
                break;
            case TD_USER:
                b_left = display_printf(&tdfbuf, "%s ",
                                        logfields->user_time_fmt);
                break;
            case TD_NONE:
            default:
                break;
        }

    }

    /* Trim trailing blank from date time format. */
    if (date_time_fmt[0] != '\0' &&
        date_time_fmt[strlen(date_time_fmt) - 1] == ' ')
        date_time_fmt[strlen(date_time_fmt) - 1] = '\0';
}

int ReturnLevelAscii(const char *LevelInAscii)
{
    int i = 0;

    for (i = 0; i < ARRAY_SIZE(tabLogLevel); i++)
        if (tabLogLevel[i].str != NULL &&
            (!strcasecmp(tabLogLevel[i].str, LevelInAscii)
             || !strcasecmp(tabLogLevel[i].str + 4, LevelInAscii)
             || !strcasecmp(tabLogLevel[i].short_str, LevelInAscii)))
            return i;

    /* If nothing found, return -1 */
    return -1;
}				/* ReturnLevelAscii */

void DisplayLogComponentLevel(log_components_t component, const char *file,
                              int line, const char *function, log_levels_t level,
                              const char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);

    display_log_component_level(component, file, line, function, level,
                                format, arguments);

    va_end(arguments);
}

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t siz)
{
    register char *d = dst;
    register const char *s = src;
    register size_t n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0 && --n != 0) {
        do {
            *d++ = *s;
            if (*s++ == 0)
                break;
        } while (--n != 0);
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0) {
            /* NUL-terminate dst */
            *d = '\0';
        }

        /* Search for NUL in string */
        while (*s++) {
            /* do nothing */
        }
    }

    /* count does not include NUL */
    return s - src - 1;
}

/*
 * Variables specifiques aux threads.
 */

__thread char thread_name[32];
__thread char log_buffer[LOG_BUFF_LEN + 1];
__thread char *clientip = NULL;

/* Set the function name in progress. */
void SetNameFunction(const char *nom)
{
    if (strlcpy(thread_name, nom, sizeof(thread_name))
        >= sizeof(thread_name)) {
        LogWarn(COMPONENT_LOG,
                "Thread name %s too long truncated to %s",
                nom, thread_name);
    }
    clientip = NULL;
}

/*
 * Set the name of this program.
 */
void SetNamePgm(const char *nom)
{

    /* This function isn't thread-safe because the name of the program
     * is common among all the threads. */
    if (strlcpy(program_name, nom, sizeof(program_name))
        >= sizeof(program_name))
        LogFatal(COMPONENT_LOG, "Program name %s too long", nom);
}

/*
 * Set the hostname.
 */
void SetNameHost(const char *name)
{
    if (strlcpy(hostname, name, sizeof(hostname))
        >= sizeof(hostname))
        LogFatal(COMPONENT_LOG, "Host name %s too long", name);
}

/**
 * @brief Initialize Logging
 *
 * Called very early in server init to make logging available as
 * soon as possible. Create a logger to stderr first and make it
 * the default.  We are forced to fprintf to stderr by hand until
 * this happens.  Once this is up, the logger is working.
 * We then get stdout and syslog loggers init'd.
 * If log_path (passed in via -L on the command line), we get a
 * FILE logger going and make it our default logger.  Otherwise,
 * we use syslog as the default.
 *
 * @param log_path    [IN] optarg from -L, otherwise NULL
 * @param debug_level [IN] global debug level from -N optarg
 */

pthread_rwlock_t log_rwlock;

struct glist_head {
    struct glist_head *next;
    struct glist_head *prev;
};

static inline void glist_init(struct glist_head *head)
{				/* XXX glist_init? */
    head->next = head;
    head->prev = head;
}


/* constants */
static int log_mask = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

static struct glist_head facility_list;
static struct glist_head active_facility_list;

static int log_to_file(log_header_t headers, void *private,
                       log_levels_t level,
                       struct display_buffer *buffer, char *compstr,
                       char *message)
{
    int fd, my_status, len, rc = 0;
    char *path = private;

    len = display_buffer_len(buffer);

    /* Add newline to end of buffer */
    buffer->b_start[len] = '\n';
    buffer->b_start[len + 1] = '\0';

    fd = open(path, O_WRONLY | O_APPEND | O_CREAT, log_mask);

    if (fd != -1) {
        rc = write(fd, buffer->b_start, len + 1);

        if (rc < (len + 1)) {
            if (rc >= 0)
                my_status = ENOSPC;
            else
                my_status = errno;

            (void)close(fd);

            goto error;
        }

        rc = close(fd);

        if (rc == 0)
            goto out;
    }

    my_status = errno;

    error:

    fprintf(stderr,
            "Error: couldn't complete write to the log file %s status=%d (%s) message was:\n%s",
            path, my_status, strerror(my_status), buffer->b_start);

    out:

    /* Remove newline from buffer */
    buffer->b_start[len] = '\0';

    return rc;
}

#if defined(__GLIBC__) && defined(_GNU_SOURCE)
#define gsh_strdupa(src) strdupa(src)
#else
#define gsh_strdupa(src) ({\
		char *dest = alloca(strlen(src) + 1); \
		strcpy(dest, src); \
		dest; \
	})
#endif

#define container_of(addr, type, member) ({			\
	const typeof(((type *) 0)->member) * __mptr = (addr);	\
	(type *)((char *) __mptr - offsetof(type, member)); })

#define glist_for_each(node, head) \
	for (node = (head)->next; node != head; node = node->next)

#define glist_entry(node, type, member) \
	container_of(node, type, member)

/**
* @brief Define the structure for a log facility.
*
*/
struct log_facility {
    struct glist_head lf_list;	/*< List of log facilities */
    struct glist_head lf_active;	/*< This is an active facility */
    char *lf_name;			/*< Name of log facility */
    log_levels_t lf_max_level;	/*< Max log level for this facility */
    log_header_t lf_headers;	/*< If time stamp etc. are part of msg
					 */
    lf_function_t *lf_func;	/*< Function that describes facility   */
    void *lf_private;	/*< Private info for facility          */
};

/**
 *
 * @brief Finds a log facility by name
 *
 * Must be called under the rwlock
 *
 * @param[in]  name The name of the facility to be found
 *
 * @retval NULL No facility by that name
 * @retval non-NULL Pointer to the facility structure
 *
 */
static struct log_facility *find_log_facility(const char *name)
{
    struct glist_head *glist;
    struct log_facility *facility;

    glist_for_each(glist, &facility_list) {
        facility = glist_entry(glist, struct log_facility, lf_list);
        if (!strcasecmp(name, facility->lf_name))
            return facility;
    }

    return NULL;
}

/**
 * @brief Create a logging facility
 *
 * A logging facility outputs log messages using the helper function
 * log_func.  See below for enabling/disabling.
 *
 * @param name       [IN] the name of the new logger
 * @param log_func   [IN] function pointer to the helper
 * @param max_level  [IN] maximum message level this logger will handle.
 * @param header     [IN] detail level for header part of messages
 * @param private    [IN] logger specific argument.
 *
 * @return 0 on success, -errno for failure
 */

#define gsh_calloc(n, s) ({ \
		void *p_ = calloc(n, s); \
		if (p_ == NULL) { \
			abort(); \
		} \
		p_; \
	})

#define gsh_strdup(s) ({ \
		char *p_ = strdup(s); \
		if (p_ == NULL) { \
			abort(); \
		} \
		p_; \
	})

/* Add the new element between left and right */
static inline void __glist_add(struct glist_head *left,
                               struct glist_head *right, struct glist_head *elt)
{
    elt->prev = left;
    elt->next = right;
    left->next = elt;
    right->prev = elt;
}

static inline void glist_add_tail(struct glist_head *head,
                                  struct glist_head *elt)
{

    __glist_add(head->prev, head, elt);
}

int create_log_facility(const char *name, lf_function_t *log_func,
                        log_levels_t max_level, log_header_t header,
                        void *private)
{
    struct log_facility *facility;

    if (name == NULL || *name == '\0')
        return -EINVAL;
    if (max_level < NIV_NULL || max_level >= NB_LOG_LEVEL)
        return -EINVAL;
    if (log_func == log_to_file && private != NULL) {
        char *dir;
        int rc;

        if (*(char *)private == '\0' ||
            strlen(private) >= MAXPATHLEN) {
            LogCrit(COMPONENT_LOG,
                    "New log file path empty or too long");
            return -EINVAL;
        }
        dir = gsh_strdupa(private);
        dir = dirname(dir);
        rc = access(dir, W_OK);
        if (rc != 0) {
            rc = errno;
            LogCrit(COMPONENT_LOG,
                    "Cannot create new log file (%s), because: %s",
                    (char *)private, strerror(rc));
            return -rc;
        }
    }
    PTHREAD_RWLOCK_wrlock(&log_rwlock);

    facility = find_log_facility(name);

    if (facility != NULL) {
        PTHREAD_RWLOCK_unlock(&log_rwlock);

        LogInfo(COMPONENT_LOG, "Facility %s already exists", name);

        return -EEXIST;
    }

    facility = gsh_calloc(1, sizeof(*facility));

    facility->lf_name = gsh_strdup(name);
    facility->lf_func = log_func;
    facility->lf_max_level = max_level;
    facility->lf_headers = header;

    if (log_func == log_to_file && private != NULL)
        facility->lf_private = gsh_strdup(private);
    else
        facility->lf_private = private;

    glist_add_tail(&facility_list, &facility->lf_list);

    PTHREAD_RWLOCK_unlock(&log_rwlock);

    LogInfo(COMPONENT_LOG, "Created log facility %s",
            facility->lf_name);

    return 0;
}

static int log_to_stream(log_header_t headers, void *private,
                         log_levels_t level,
                         struct display_buffer *buffer, char *compstr,
                         char *message)
{
    FILE *stream = private;
    int rc;
    char *msg = buffer->b_start;
    int len;

    len = display_buffer_len(buffer);

    /* Add newline to end of buffer */
    buffer->b_start[len] = '\n';
    buffer->b_start[len + 1] = '\0';

    switch (headers) {
        case LH_NONE:
            msg = message;
            break;
        case LH_COMPONENT:
            msg = compstr;
            break;
        case LH_ALL:
            msg = buffer->b_start;
            break;
        default:
            msg = "Somehow header level got messed up!!";
    }

    rc = fputs(msg, stream);

    if (rc != EOF)
        rc = fflush(stream);

    /* Remove newline from buffer */
    buffer->b_start[len] = '\0';

    if (rc == EOF)
        return -1;
    else
        return 0;
}

static struct log_facility *default_facility;

static inline void glist_del(struct glist_head *node)
{
    struct glist_head *left = node->prev;
    struct glist_head *right = node->next;

    if (left != NULL)
        left->next = right;
    if (right != NULL)
        right->prev = left;
    node->next = NULL;
    node->prev = NULL;
}

/**
 * @brief Test if this node is not on a list.
 *
 * NOT to be confused with glist_empty which is just
 * for heads.  We poison with NULL for disconnected nodes.
 */

static inline int glist_null(struct glist_head *head)
{
    return (head->next == NULL) && (head->prev == NULL);
}

log_header_t max_headers = LH_COMPONENT;

/**
 * @brief Set the named logger as the default logger
 *
 * The default logger can not be released sp we set another one as
 * the default instead.  The previous default logger is disabled.
 *
 * @param name [IN] the name of the logger to enable
 *
 * @return 0 on success, -errno on errors.
 */

static int set_default_log_facility(const char *name)
{
    struct log_facility *facility;

    if (name == NULL || *name == '\0')
        return -EINVAL;

    PTHREAD_RWLOCK_wrlock(&log_rwlock);
    facility = find_log_facility(name);
    if (facility == NULL) {
        PTHREAD_RWLOCK_unlock(&log_rwlock);
        LogCrit(COMPONENT_LOG, "Facility %s does not exist", name);
        return -ENOENT;
    }
    if (facility == default_facility)
        goto out;
    if (glist_null(&facility->lf_active))
        glist_add_tail(&active_facility_list, &facility->lf_active);
    if (default_facility != NULL) {
        assert(!glist_null(&default_facility->lf_active));
        glist_del(&default_facility->lf_active);
        if (facility->lf_headers != max_headers) {
            struct glist_head *glist;
            struct log_facility *found;

            max_headers = LH_NONE;
            glist_for_each(glist, &active_facility_list) {
                found = glist_entry(glist,
                                    struct log_facility,
                                    lf_active);
                if (found->lf_headers > max_headers)
                    max_headers = found->lf_headers;
            }
        }
    } else if (facility->lf_headers > max_headers)
        max_headers = facility->lf_headers;
    default_facility = facility;
    out:
    PTHREAD_RWLOCK_unlock(&log_rwlock);
    return 0;
}

static int syslog_opened;

/*
 * Routines for managing error messages
 */
static int log_to_syslog(log_header_t headers, void *private,
                         log_levels_t level,
                         struct display_buffer *buffer, char *compstr,
                         char *message)
{
    if (!syslog_opened) {
        openlog("nfs-ganesha", LOG_PID, LOG_USER);
        syslog_opened = 1;
    }

    /* Writing to syslog. */
    syslog(tabLogLevel[level].syslog_level, "%s", compstr);

    return 0;
}


/* threads keys */
#define LogChanges(format, args...) \
	DisplayLogComponentLevel(COMPONENT_LOG, \
				 __FILE__, \
				 __LINE__, \
				 __func__, \
				 NIV_NULL, \
				 "LOG: " format, \
				 ## args)

static void SetLevelDebug(int level_to_set)
{
    int i;

    if (level_to_set < NIV_NULL)
        level_to_set = NIV_NULL;

    if (level_to_set >= NB_LOG_LEVEL)
        level_to_set = NB_LOG_LEVEL - 1;

    /* COMPONENT_ALL is a pseudo component, handle it separately */
    component_log_level[COMPONENT_ALL] = level_to_set;
    for (i = COMPONENT_ALL + 1; i < COMPONENT_COUNT; i++) {
        SetComponentLogLevel(i, level_to_set);
    }
}				/* _SetLevelDebug */

void init_logging(const char *log_path, const int debug_level)
{
    int rc;

    /* Finish initialization of and register log facilities. */
    PTHREAD_RWLOCK_init(&log_rwlock, NULL);
#ifdef _DONT_HAVE_LOCALTIME_R
    PTHREAD_MUTEX_init(&mutex_localtime, NULL);
#endif
    glist_init(&facility_list);
    glist_init(&active_facility_list);

    /* Initialize const_log_str to defaults. Ganesha can start logging
     * before the LOG config is processed (in fact, LOG config can itself
     * issue log messages to indicate errors.
     */
    set_const_log_str();

    rc = create_log_facility("STDERR", log_to_stream,
                             NIV_FULL_DEBUG, LH_ALL, stderr);
    if (rc != 0) {
        fprintf(stderr, "Create error (%s) for STDERR log facility!",
                strerror(-rc));
        Fatal();
    }
    rc = set_default_log_facility("STDERR");
    if (rc != 0) {
        fprintf(stderr, "Enable error (%s) for STDERR log facility!",
                strerror(-rc));
        Fatal();
    }
    rc = create_log_facility("STDOUT", log_to_stream,
                             NIV_FULL_DEBUG, LH_ALL, stdout);
    if (rc != 0)
        LogFatal(COMPONENT_LOG,
                 "Create error (%s) for STDOUT log facility!",
                 strerror(-rc));
    rc = create_log_facility("SYSLOG", log_to_syslog,
                             NIV_FULL_DEBUG, LH_COMPONENT, NULL);
    if (rc != 0)
        LogFatal(COMPONENT_LOG,
                 "Create error (%s) for SYSLOG log facility!",
                 strerror(-rc));

    if (log_path) {
        if ((strcmp(log_path, "STDERR") == 0) ||
            (strcmp(log_path, "SYSLOG") == 0) ||
            (strcmp(log_path, "STDOUT") == 0)) {
            rc = set_default_log_facility(log_path);
            if (rc != 0)
                LogFatal(COMPONENT_LOG,
                         "Enable error (%s) for %s logging!",
                         strerror(-rc), log_path);
        } else {
            rc = create_log_facility("FILE", log_to_file,
                                     NIV_FULL_DEBUG, LH_ALL,
                                     (void *)log_path);
            if (rc != 0)
                LogFatal(COMPONENT_LOG,
                         "Create error (%s) for FILE (%s) logging!",
                         strerror(-rc), log_path);
            rc = set_default_log_facility("FILE");
            if (rc != 0)
                LogFatal(COMPONENT_LOG,
                         "Enable error (%s) for FILE (%s) logging!",
                         strerror(-rc), log_path);
        }
    } else {
        /* Fall back to SYSLOG as the first default facility */
        rc = set_default_log_facility("SYSLOG");
        if (rc != 0)
            LogFatal(COMPONENT_LOG,
                     "Enable error (%s) for SYSLOG logging!",
                     strerror(-rc));
    }

    if (debug_level >= 0) {
        LogChanges("Setting log level for all components to %s",
                   ReturnLevelInt(debug_level));
        SetLevelDebug(debug_level);
        original_log_level = debug_level;
    }
}

struct cleanup_list_element *cleanup_list;

void Cleanup(void)
{
    struct cleanup_list_element *c = cleanup_list;

    while (c != NULL) {
        c->clean();
        c = c->next;
    }

    PTHREAD_RWLOCK_destroy(&log_rwlock);
#ifdef _DONT_HAVE_LOCALTIME_R
    PTHREAD_MUTEX_destroy(&mutex_localtime);
#endif
}


/*
 *  Re-export component logging to TI-RPC internal logging
 */
void rpc_warnx(char *fmt, ...)
{
    va_list ap;

    if (component_log_level[COMPONENT_TIRPC] <= NIV_FATAL)
        return;

    va_start(ap, fmt);

    display_log_component_level(COMPONENT_TIRPC, "<no-file>", 0, "rpc",
                                component_log_level[COMPONENT_TIRPC],
                                fmt, ap);

    va_end(ap);
}

static bool disp_utc_timestamp;
#define Localtime_r localtime_r

int display_timeval(struct display_buffer *dspbuf, struct timeval *tv)
{
    char *fmt = date_time_fmt;
    int b_left = display_start(dspbuf);
    struct tm the_date;
    char tbuf[MAX_TD_FMT_LEN];
    time_t tm = tv->tv_sec;

    if (b_left <= 0)
        return b_left;

    if (logfields->datefmt == TD_NONE && logfields->timefmt == TD_NONE)
        fmt = "%c ";

    if (disp_utc_timestamp)
        gmtime_r(&tm, &the_date);
    else
        Localtime_r(&tm, &the_date);

    /* Earlier we build the date/time format string in
     * date_time_fmt, now use that to format the time and/or date.
     * If time format is TD_SYSLOG_USEC, then we need an additional
     * step to add the microseconds (since strftime just takes a
     * struct tm which was filled in from a time_t and thus does not
     * have microseconds.
     */
    if (strftime(tbuf, sizeof(tbuf), fmt, &the_date) != 0) {
        if (logfields->timefmt == TD_SYSLOG_USEC)
            b_left = display_printf(dspbuf, tbuf, tv->tv_usec);
        else
            b_left = display_cat(dspbuf, tbuf);
    }

    return b_left;
}


/**
 * @brief Reset current position in buffer to start.
 *
 * @param[in,out] dspbuf The buffer.
 *
 */
static inline void display_reset_buffer(struct display_buffer *dspbuf)
{
    /* To re-use a buffer, all we need to do is roll b_current back to
     * b_start and make it empty.
     */
    dspbuf->b_current = dspbuf->b_start;
    *dspbuf->b_current = '\0';
}

static int display_log_header(struct display_buffer *dsp_log)
{
    int b_left = display_start(dsp_log);

    if (b_left <= 0 || max_headers < LH_ALL)
        return b_left;

    /* Print date and/or time if either flag is enabled. */
    if (b_left > 0
        && (logfields->datefmt != TD_NONE
            || logfields->timefmt != TD_NONE)) {
        struct timeval tv;

        if (logfields->timefmt == TD_SYSLOG_USEC) {
            gettimeofday(&tv, NULL);
        } else {
            tv.tv_sec = time(NULL);
            tv.tv_usec = 0;
        }

        b_left = display_timeval(dsp_log, &tv);

        if (b_left > 0)
            b_left = display_cat(dsp_log, " ");
    }

    if (b_left > 0 && const_log_str[0] != '\0')
        b_left = display_cat(dsp_log, const_log_str);

    /* If thread name will not follow, need a : separator */
    if (b_left > 0 && !logfields->disp_threadname)
        b_left = display_cat(dsp_log, ": ");

    /* If we overflowed the buffer with the header, just skip it. */
    if (b_left == 0) {
        display_reset_buffer(dsp_log);
        b_left = display_start(dsp_log);
    }

    /* The message will now start at dsp_log.b_current */
    return b_left;
}

static int display_log_component(struct display_buffer *dsp_log,
                                 log_components_t component, const char *file,
                                 int line, const char *function, int level)
{
    int b_left = display_start(dsp_log);

    if (b_left <= 0 || max_headers < LH_COMPONENT)
        return b_left;

    if (b_left > 0 && logfields->disp_clientip) {
        if (clientip)
            b_left = display_printf(dsp_log, "[%s] ",
                                    clientip);
        else
            b_left = display_printf(dsp_log, "[none] ");
    }

    if (b_left > 0 && logfields->disp_threadname) {
        if (thread_name[0] != '\0')
            b_left = display_printf(dsp_log, "[%s] ",
                                    thread_name);
        else
            b_left = display_printf(dsp_log, "[%p] ",
                                    thread_name);
    }

    if (b_left > 0 && logfields->disp_filename) {
        if (logfields->disp_linenum)
            b_left = display_printf(dsp_log, "%s:", file);
        else
            b_left = display_printf(dsp_log, "%s :", file);
    }

    if (b_left > 0 && logfields->disp_linenum)
        b_left = display_printf(dsp_log, "%d :", line);

    if (b_left > 0 && logfields->disp_funct)
        b_left = display_printf(dsp_log, "%s :", function);

    if (b_left > 0 && logfields->disp_comp)
        b_left =
                display_printf(dsp_log, "%s :",
                               LogComponents[component].comp_str);

    if (b_left > 0 && logfields->disp_level)
        b_left =
                display_printf(dsp_log, "%s :",
                               tabLogLevel[level].short_str);

    if (b_left > 0 && logfields->disp_op_id) {
        b_left = display_printf(dsp_log, "op_id=none :");
    }

    /* If we overflowed the buffer with the header, just skip it. */
    if (b_left == 0) {
        display_reset_buffer(dsp_log);
        b_left = display_start(dsp_log);
    }

    return b_left;
}

void display_log_component_level(log_components_t component, const char *file,
                                 int line, const char *function,
                                 log_levels_t level, const char *format,
                                 va_list arguments)
{
    char *compstr;
    char *message;
    int b_left;
    struct glist_head *glist;
    struct log_facility *facility;
    struct display_buffer dsp_log = {sizeof(log_buffer),
                                     log_buffer, log_buffer};

    /* Build up the message and capture the various positions in it. */
    b_left = display_log_header(&dsp_log);

    if (b_left > 0)
        compstr = dsp_log.b_current;
    else
        compstr = dsp_log.b_start;

    if (b_left > 0)
        b_left =
                display_log_component(&dsp_log, component, file, line,
                                      function, level);

    if (b_left > 0)
        message = dsp_log.b_current;
    else
        message = dsp_log.b_start;

    if (b_left > 0)
        b_left = display_vprintf(&dsp_log, format, arguments);

#ifdef USE_LTTNG
        tracepoint(ganesha_logger, log,
		   component, level, file, line, function, message);
#endif

    PTHREAD_RWLOCK_rdlock(&log_rwlock);

    glist_for_each(glist, &active_facility_list) {
        facility = glist_entry(glist, struct log_facility, lf_active);

        if (level <= facility->lf_max_level
            && facility->lf_func != NULL)
            facility->lf_func(facility->lf_headers,
                              facility->lf_private,
                              level, &dsp_log,
                              compstr, message);
    }

    PTHREAD_RWLOCK_unlock(&log_rwlock);

    if (level == NIV_FATAL)
        Fatal();
}

struct log_component_info LogComponents[COMPONENT_COUNT] = {
        [COMPONENT_ALL] = {
                .comp_name = "COMPONENT_ALL",
                .comp_str = "",},
        [COMPONENT_LOG] = {
                .comp_name = "COMPONENT_LOG",
                .comp_str = "LOG",},
        [COMPONENT_MEM_ALLOC] = {
                .comp_name = "COMPONENT_MEM_ALLOC",
                .comp_str = "MEM ALLOC",},
        [COMPONENT_MEMLEAKS] = {
                .comp_name = "COMPONENT_MEMLEAKS",
                .comp_str = "LEAKS",},
        [COMPONENT_FSAL] = {
                .comp_name = "COMPONENT_FSAL",
                .comp_str = "FSAL",},
        [COMPONENT_NFSPROTO] = {
                .comp_name = "COMPONENT_NFSPROTO",
                .comp_str = "NFS3",},
        [COMPONENT_NFS_V4] = {
                .comp_name = "COMPONENT_NFS_V4",
                .comp_str = "NFS4",},
        [COMPONENT_EXPORT] = {
                .comp_name = "COMPONENT_EXPORT",
                .comp_str = "EXPORT",},
        [COMPONENT_FILEHANDLE] = {
                .comp_name = "COMPONENT_FILEHANDLE",
                .comp_str = "FH",},
        [COMPONENT_DISPATCH] = {
                .comp_name = "COMPONENT_DISPATCH",
                .comp_str = "DISP",},
        [COMPONENT_MDCACHE] = {
                .comp_name = "COMPONENT_MDCACHE",
                .comp_str = "MDCACHE",},
        [COMPONENT_MDCACHE_LRU] = {
                .comp_name = "COMPONENT_MDCACHE_LRU",
                .comp_str = "MDCACHE LRU",},
        [COMPONENT_HASHTABLE] = {
                .comp_name = "COMPONENT_HASHTABLE",
                .comp_str = "HT",},
        [COMPONENT_HASHTABLE_CACHE] = {
                .comp_name = "COMPONENT_HASHTABLE_CACHE",
                .comp_str = "HT CACHE",},
        [COMPONENT_DUPREQ] = {
                .comp_name = "COMPONENT_DUPREQ",
                .comp_str = "DUPREQ",},
        [COMPONENT_INIT] = {
                .comp_name = "COMPONENT_INIT",
                .comp_str = "NFS STARTUP",},
        [COMPONENT_MAIN] = {
                .comp_name = "COMPONENT_MAIN",
                .comp_str = "MAIN",},
        [COMPONENT_IDMAPPER] = {
                .comp_name = "COMPONENT_IDMAPPER",
                .comp_str = "ID MAPPER",},
        [COMPONENT_NFS_READDIR] = {
                .comp_name = "COMPONENT_NFS_READDIR",
                .comp_str = "NFS READDIR",},
        [COMPONENT_NFS_V4_LOCK] = {
                .comp_name = "COMPONENT_NFS_V4_LOCK",
                .comp_str = "NFS4 LOCK",},
        [COMPONENT_CONFIG] = {
                .comp_name = "COMPONENT_CONFIG",
                .comp_str = "CONFIG",},
        [COMPONENT_CLIENTID] = {
                .comp_name = "COMPONENT_CLIENTID",
                .comp_str = "CLIENT ID",},
        [COMPONENT_SESSIONS] = {
                .comp_name = "COMPONENT_SESSIONS",
                .comp_str = "SESSIONS",},
        [COMPONENT_PNFS] = {
                .comp_name = "COMPONENT_PNFS",
                .comp_str = "PNFS",},
        [COMPONENT_RW_LOCK] = {
                .comp_name = "COMPONENT_RW_LOCK",
                .comp_str = "RW LOCK",},
        [COMPONENT_NLM] = {
                .comp_name = "COMPONENT_NLM",
                .comp_str = "NLM",},
        [COMPONENT_RPC] = {
                .comp_name = "COMPONENT_RPC",
                .comp_str = "RPC",},
        [COMPONENT_TIRPC] = {
                .comp_name = "COMPONENT_TIRPC",
                .comp_str = "TIRPC",},
        [COMPONENT_NFS_CB] = {
                .comp_name = "COMPONENT_NFS_CB",
                .comp_str = "NFS CB",},
        [COMPONENT_THREAD] = {
                .comp_name = "COMPONENT_THREAD",
                .comp_str = "THREAD",},
        [COMPONENT_NFS_V4_ACL] = {
                .comp_name = "COMPONENT_NFS_V4_ACL",
                .comp_str = "NFS4 ACL",},
        [COMPONENT_STATE] = {
                .comp_name = "COMPONENT_STATE",
                .comp_str = "STATE",},
        [COMPONENT_9P] = {
                .comp_name = "COMPONENT_9P",
                .comp_str = "9P",},
        [COMPONENT_9P_DISPATCH] = {
                .comp_name = "COMPONENT_9P_DISPATCH",
                .comp_str = "9P DISP",},
        [COMPONENT_FSAL_UP] = {
                .comp_name = "COMPONENT_FSAL_UP",
                .comp_str = "FSAL_UP",},
        [COMPONENT_DBUS] = {
                .comp_name = "COMPONENT_DBUS",
                .comp_str = "DBUS",},
        [COMPONENT_NFS_MSK] = {
                .comp_name = "COMPONENT_NFS_MSK",
                .comp_str = "NFS_MSK",},
};

tirpc_pkg_params ntirpc_pp;

uint32_t rpc_debug_flags = TIRPC_DEBUG_FLAG_ERROR |
                           TIRPC_DEBUG_FLAG_WARN |
                           TIRPC_DEBUG_FLAG_EVENT;

static void SetNTIRPCLogLevel(int level_to_set)
{
    uint32_t old = ntirpc_pp.debug_flags;

    switch (level_to_set) {
        case NIV_NULL:
        case NIV_FATAL:
            ntirpc_pp.debug_flags = 0; /* disable all flags */
            break;
        case NIV_CRIT:
        case NIV_MAJ:
            ntirpc_pp.debug_flags = TIRPC_DEBUG_FLAG_ERROR;
            break;
        case NIV_WARN:
            ntirpc_pp.debug_flags = TIRPC_DEBUG_FLAG_ERROR |
                                    TIRPC_DEBUG_FLAG_WARN;
            break;
        case NIV_EVENT:
        case NIV_INFO:
            ntirpc_pp.debug_flags = TIRPC_DEBUG_FLAG_ERROR |
                                    TIRPC_DEBUG_FLAG_WARN |
                                    TIRPC_DEBUG_FLAG_EVENT;
            break;
        case NIV_DEBUG:
        case NIV_MID_DEBUG:
            /* set by log_conf_commit() */
            ntirpc_pp.debug_flags = rpc_debug_flags;
            break;
        case NIV_FULL_DEBUG:
            ntirpc_pp.debug_flags = 0xFFFFFFFF; /* enable all flags */
            break;
        default:
            ntirpc_pp.debug_flags = TIRPC_DEBUG_FLAG_DEFAULT;
            break;
    }

    if (!tirpc_control(TIRPC_SET_DEBUG_FLAGS, &ntirpc_pp.debug_flags))
        LogCrit(COMPONENT_CONFIG, "Setting nTI-RPC debug_flags failed");
    else if (old != ntirpc_pp.debug_flags)
        LogChanges("Changed RPC_Debug_Flags from %"PRIx32" to %"PRIx32,
                   old, ntirpc_pp.debug_flags);
}

void SetComponentLogLevel(log_components_t component, int level_to_set)
{

    assert(level_to_set >= NIV_NULL);
    assert(level_to_set < NB_LOG_LEVEL);
    assert(component != COMPONENT_ALL);

    if (component_log_level[component] == level_to_set)
        return;

    LogChanges("Changing log level of %s from %s to %s",
               LogComponents[component].comp_name,
               ReturnLevelInt(component_log_level[component]),
               ReturnLevelInt(level_to_set));
    component_log_level[component] = level_to_set;

    if (component == COMPONENT_TIRPC)
        SetNTIRPCLogLevel(level_to_set);
}

void Fatal(void)
{
    Cleanup();
    _exit(2);
}

char *ReturnLevelInt(int level)
{
    if (level >= 0 && level < NB_LOG_LEVEL)
        return tabLogLevel[level].str;

    /* If nothing is found, return NULL. */
    return NULL;
}				/* ReturnLevelInt */