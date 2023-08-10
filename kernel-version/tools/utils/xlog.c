/*
 * support/nfs/xlog.c
 *
 * This module handles the logging of requests.
 *
 * TODO:	Merge the two "XXX_log() calls.
 *
 * Authors:	Donald J. Becker, <becker@super.org>
 *		Rick Sladkey, <jrs@world.std.com>
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Olaf Kirch, <okir@monad.swb.de>
 *
 *		This software maybe be used for any purpose provided
 *		the above copyright notice is retained.  It is supplied
 *		as is, with no warranty expressed or implied.
 */
/*系统日志位置 /var/log/syslog*/
/*通过二进制的交并判断日志类别是否存在,每个日志级别转为二进制都必须独占一位这样才可以进行唯一判定，尤其是debug模式下的日志级别
 * 如  D_CALL 0x0002 二进制为 0010
 *    D_AUTH  0x0004 二进制为 0100
 *    这样由模式掩码 logmask 进行|=添加日志级别在判定时进行&运算就可以判定日志级别是否开启*/
/*判断是否需要读取配置*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>
#include "nfslib.h"

/*当log_stderr为1（打开std输出模式），设置此定义，日志输出添加记录日期*/
#undef    VERBOSE_PRINTF

/*std输出模式开关，设置为0则为关，其他数字为开*/
static int log_stderr = 1;

/*系统日志开关，设置为0则为关，其他数字为开*/
static int log_syslog = 1;

//打开debug下的日志,要输出debug日志必须关闭std模式，其全为info级别
static int logging = 0;        /* enable/disable DEBUG logs	*/

//限制debug日志的输出模式
static int logmask = 0;        /* What will be logged		*/

//调用日志的程序名
static char log_name[256];        /* name of this program		*/

//日志进程id
static int log_pid = -1;        /* PID of this program		*/

//暴露出错误
int export_errno = 0;

//信号执行方法
static void xlog_toggle(int sig);

//debug模式下的級別
static struct xlog_debugfac debugnames[] = {
        {"general", D_GENERAL,},
        {"call",    D_CALL,},
        {"auth",    D_AUTH,},
        {"parse",   D_PARSE,},
        {"all",     D_ALL,},
        {NULL, 0,},
};

/*建立日志,建立信号监听*/
void
xlog_open(char *progname) {
    openlog(progname, LOG_PID, LOG_DAEMON);

    strncpy(log_name, progname, sizeof(log_name) - 1);
    log_name[sizeof(log_name) - 1] = '\0';
    log_pid = getpid();

    signal(SIGUSR1, xlog_toggle);
    signal(SIGUSR2, xlog_toggle);
}

/*std模式设置*/
void
xlog_stderr(int on) {
    log_stderr = on;
}

/*系统日志模式设置*/
void
xlog_syslog(int on) {
    log_syslog = on;
}

/*信号监听程序，当获取了user1信号，则判定开启debug模式下的日志级别，获取了user2信号关闭debug模式日志*/
static void
xlog_toggle(int sig) {
    unsigned int tmp, i;
    if (sig == SIGUSR1) {
        if ((logmask & D_ALL) && !logging) {
            xlog(D_GENERAL, "turned on logging");
            logging = 1;
            return;
        }
        tmp = ~logmask;
        logmask |= ((logmask & D_ALL) << 1) | D_GENERAL;
        for (i = -1, tmp &= logmask; tmp; tmp >>= 1, i++)
            if (tmp & 1)
                xlog(D_GENERAL,
                     "turned on logging level %d", i);
    } else {
        xlog(D_GENERAL, "turned off logging");
        logging = 0;
    }
    signal(sig, xlog_toggle);
}

/*设置各个日志级别的开启关闭，fac为各个日志的级别，当on为0关闭，其他开启*/
void
xlog_config(int fac, int on) {
    if (on)
        logmask |= fac;
    else
        logmask &= ~fac;
    if (on)
        logging = 1;
}

/*设置debug级别的日志，kind是debugnames下df_name，on为0表示关闭，其他为开启*/
void
xlog_sconfig(char *kind, int on) {
    struct xlog_debugfac *tbl = debugnames;

    while (tbl->df_name != NULL && strcasecmp(tbl->df_name, kind))
        tbl++;
    if (!tbl->df_name) {
        xlog(L_WARNING, "Invalid debug facility: %s\n", kind);
        return;
    }
    xlog_config(tbl->df_fac, on);
}

/*判断debug模式下的日志级别是否开启*/
int
xlog_enabled(int fac) {
    return (logging && (fac & logmask));
}


/* 将日志打印到系统日志和std模式下（根据开关和级别进行打印），debug需要保证没有开启std模式，则按照info级别打印，当使用L_FATAL时终止程序*/
void
xlog_backend(int kind, const char *fmt, va_list args) {
    va_list args2;
    /*这里先判定日志级别是否开启*/
    if (!(kind & (L_ALL)) && !(logging && (kind & logmask)))
        return;

    /*如果设置了std模式，复制参数*/
    if (log_stderr)
        va_copy(args2, args);

    /*判断日志级别进行打印，对于debug模式的级别一律按照info打印，并且保证没有开启std模式*/
    if (log_syslog) {
        switch (kind) {
            case L_FATAL:
                vsyslog(LOG_ERR, fmt, args);
                break;
            case L_ERROR:
                vsyslog(LOG_ERR, fmt, args);
                break;
            case L_WARNING:
                vsyslog(LOG_WARNING, fmt, args);
                break;
            case L_NOTICE:
                vsyslog(LOG_NOTICE, fmt, args);
                break;
            default:
                if (!log_stderr)
                    vsyslog(LOG_INFO, fmt, args);
                break;
        }
    }

    /*std模式打印*/
    if (log_stderr) {
#ifdef VERBOSE_PRINTF
        time_t		now;
        struct tm	*tm;

        time(&now);
        tm = localtime(&now);
        fprintf(stderr, "%s[%d] %04d-%02d-%02d %02d:%02d:%02d ",
                log_name, log_pid,
                tm->tm_year+1900, tm->tm_mon + 1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec);
#else
        fprintf(stderr, "%s: ", log_name);
#endif
        vfprintf(stderr, fmt, args2);
        fprintf(stderr, "\n");
        va_end(args2);
    }

    if (kind == L_FATAL)
        exit(1);
}

/*打印日志，export_erron我认为是判定是不是打印了错误日志的标志，供其他程序获取日志状态，kind是打印的类型，*fmt是打印的格式,...是符合格式的可变参数：
 * xlog(LC_ERROR, "[error]:%s//%d","test",12);*/
void
xlog(int kind, const char *fmt, ...) {
    va_list args;

    if (kind & (L_ERROR | D_GENERAL))
        export_errno = 1;

    va_start(args, fmt);
    xlog_backend(kind, fmt, args);
    va_end(args);
}

/*打印程序警告,*fmt是打印的格式,...是符合格式的可变参数*/
void
xlog_warn(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    xlog_backend(L_WARNING, fmt, args);
    va_end(args);
}

/*打印错误日志，并退出程序,*fmt是打印的格式,...是符合格式的可变参数*/
void
xlog_err(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    xlog_backend(L_FATAL, fmt, args);
    va_end(args);
}

/*打印错误日志，更新错误码，并退出程序,*fmt是打印的格式,...是符合格式的可变参数*/
void
xlog_errno(int err, const char *fmt, ...) {
    va_list args;

    errno = err;
    va_start(args, fmt);
    xlog_backend(L_FATAL, fmt, args);
    va_end(args);
}
