
#ifndef NTIRPC_PORTABLE_H
#define NTIRPC_PORTABLE_H

#include <misc/timespec.h>
#include <misc/os_epoll.h>	/* before rpc.h */

#if BSDBASED
#include <netinet/in.h>

#define SOL_IP		IPPROTO_IP
#define SOL_IPV6	IPPROTO_IPV6
#endif

#if defined(__FreeBSD__)

#define IP_PKTINFO IP_RECVIF

struct in_pktinfo {
	struct in_addr ipi_addr;	/* destination IPv4 address */
	int ipi_ifindex;	/* received interface index */
};

/* YES.  Move. */
#define HAVE_PEEREID 1

#endif

#if defined(__linux__)

/* POSIX clocks */
#define CLOCK_REALTIME_FAST CLOCK_REALTIME_COARSE
#define CLOCK_MONOTONIC_FAST CLOCK_MONOTONIC_COARSE

/* poll */
#define POLLRDNORM     0x040	/* Normal data may be read.  */
#define POLLRDBAND     0x080	/* Priority data may be read.  */

#define HAVE_GETPEEREID 0

#endif

#if defined(_WIN32)
#ifdef _MSC_VER
#include <misc/stdint.h>
#else
#include <stdint.h>
#endif
#include <reentrant.h>
#include <misc/timespec.h>

#define CLOCK_MONOTONIC_FAST 6

typedef uint32_t clockid_t;
extern int clock_gettime(clockid_t clock, struct timespec *ts);

void warnx(const char *fmt, ...);

#else
#define PtrToUlong(addr) ((unsigned long)(addr))
#endif				/* !_WIN32 */

#ifdef __APPLE__
#include <time.h>
#define CLOCK_REALTIME_FAST CLOCK_REALTIME
#define CLOCK_MONOTONIC_FAST CLOCK_MONOTONIC
#endif

#if !defined(CACHE_LINE_SIZE)
#if defined(__PPC64__)
#define CACHE_LINE_SIZE 128
#else /* __x86_64__, __i386__ and others */
#define CACHE_LINE_SIZE 64
#endif
#endif

#define CACHE_PAD(_n) char __pad ## _n [CACHE_LINE_SIZE]

/* Define bswap_## on non-GNU systems. */
#if defined(_MSC_VER)

#include <stdlib.h>
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

/* macOS / Darwin features */
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_16(x)     bswap16((x))
#define bswap_32(x)     bswap32((x))
#define bswap_64(x)     bswap64((x))

#else

/* Must be on Linux. GNU supplies bswap_## directly. */
#include <byteswap.h>

#endif				/* bswap_## */

#endif				/* NTIRPC_PORTABLE_H */
