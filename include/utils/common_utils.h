#include <assert.h>
#include <stdlib.h>

#include "log/log.h"

extern pthread_mutexattr_t default_mutex_attr;
extern pthread_rwlockattr_t default_rwlock_attr;

#ifndef __OPTIMIZE__
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#else
extern int __build_bug_on_failed;
#define BUILD_BUG_ON(condition)					\
	do {							\
		((void)sizeof(char[1 - 2*!!(condition)]));      \
		if (condition)					\
			__build_bug_on_failed = 1;		\
	} while (0)
#endif

/* Most machines scandir callback requires a const. But not all */
#define SCANDIR_CONST const

/* Most machines have mntent.h. */
#define HAVE_MNTENT_H 1

#if defined(__APPLE__)
#define pthread_yield() pthread_yield_np()
#undef SCANDIR_CONST
#define SCANDIR_CONST
#undef HAVE_MNTENT_H
#endif

#if defined(__FreeBSD__)
#undef SCANDIR_CONST
#define SCANDIR_CONST
#endif

/**
 * @brief Logging pthread attribute initialization
 *
 * @param[in,out] _attr The attributes to initialize
 */
#define PTHREAD_ATTR_init(_attr)					\
	do {								\
		int rc;							\
									\
		rc = pthread_attr_init(_attr);				\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init pthread attr %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, pthread attr init %p (%s) "	\
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging pthread attribute destruction
 *
 * @param[in,out] _attr The attributes to initialize
 */
#define PTHREAD_ATTR_destroy(_attr)					\
	do {								\
		int rc;							\
									\
		rc = pthread_attr_destroy(_attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy pthread attr %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, pthread attr destroy %p (%s) " \
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging pthread attr set scope
 *
 * @param[in,out] _attr   The attributes to update
 * @param[in]     _scope  The scope to set
 */
#define PTHREAD_ATTR_setscope(_attr, _scope)				\
	do {								\
		int rc;							\
									\
		rc = pthread_attr_setscope(_attr, _scope);		\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "pthread_attr_setscope %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, pthread_attr_setscope %p (%s) " \
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging pthread attr set detach state
 *
 * @param[in,out] _attr    The attributes to update
 * @param[in]     _detach  The detach type to set
 */
#define PTHREAD_ATTR_setdetachstate(_attr, _detach)			\
	do {								\
		int rc;							\
									\
		rc = pthread_attr_setdetachstate(_attr, _detach);	\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "pthread_attr_setdetachstate %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, pthread_attr_setdetachstate %p (%s) " \
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging pthread attr set stack size
 *
 * @param[in,out] _attr    The attributes to update
 * @param[in]     _detach  The detach type to set
 */
#define PTHREAD_ATTR_setstacksize(_attr, _detach)			\
	do {								\
		int rc;							\
									\
		rc = pthread_attr_setstacksize(_attr, _detach);	\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "pthread_attr_setstacksize %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, pthread_attr_setstacksize %p (%s) " \
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging pthread rwlock attribute initialization
 *
 * @param[in,out] _attr The attributes to initialize
 */
#define PTHREAD_RWLOCKATTR_init(_attr)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlockattr_init(_attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init rwlockattr %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, rwlockattr init %p (%s) "	\
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging pthread rwlock set kind
 *
 * @param[in,out] _attr The attributes to update
 * @param[in]     _kind The kind to set
 */
#ifdef GLIBC
#define PTHREAD_RWLOCKATTR_setkind_np(_attr, _kind)			\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlockattr_setkind_np(_attr, _kind);	\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "pthread_rwlockattr_setkind_np %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, rwlockattr setkind_np %p (%s) " \
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)
#else
#define PTHREAD_RWLOCKATTR_setkind_np(_attr, _kind) /**/
#endif

/**
 * @brief Logging pthread rwlock attribute destruction
 *
 * @param[in,out] _attr The attributes to initialize
 */
#define PTHREAD_RWLOCKATTR_destroy(_attr)				\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlockattr_destroy(_attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy rwlockattr %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, rwlockattr destroy %p (%s) " \
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging rwlock initialization
 *
 * @param[in,out] _lock The rwlock to initialize
 * @param[in,out] _attr The attributes used while initializing the lock
 */
#define PTHREAD_RWLOCK_init(_lock, _attr)				\
	do {								\
		int rc;							\
		pthread_rwlockattr_t *attr = _attr;			\
									\
		if (attr == NULL)					\
			attr = &default_rwlock_attr;			\
									\
		rc = pthread_rwlock_init(_lock, attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init rwlock %p (%s) at %s:%d",	\
				     _lock, #_lock,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Init rwlock %p (%s) "	\
				"at %s:%d", rc, _lock, #_lock,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging rwlock destroy
 *
 * @param[in,out] _lock The rwlock to destroy
 */
#define PTHREAD_RWLOCK_destroy(_lock)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_destroy(_lock);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy mutex %p (%s) at %s:%d",	\
				     _lock, #_lock,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Destroy mutex %p (%s) "	\
				"at %s:%d", rc, _lock, #_lock,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging write-lock
 *
 * @param[in,out] _lock Read-write lock
 */

#define PTHREAD_RWLOCK_wrlock(_lock)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_wrlock(_lock);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Got write lock on %p (%s) "	\
				     "at %s:%d", _lock, #_lock,		\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, write locking %p (%s) "	\
				"at %s:%d", rc, _lock, #_lock,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)							\

/**
 * @brief Logging read-lock
 *
 * @param[in,out] _lock Read-write lock
 */

#define PTHREAD_RWLOCK_rdlock(_lock)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_rdlock(_lock);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Got read lock on %p (%s) "	\
				     "at %s:%d", _lock, #_lock,		\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, read locking %p (%s) "	\
				"at %s:%d", rc, _lock, #_lock,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)							\

/**
 * @brief Logging read-write lock unlock
 *
 * @param[in,out] _lock Read-write lock
 */

#define PTHREAD_RWLOCK_unlock(_lock)					\
	do {								\
		int rc;							\
									\
		rc = pthread_rwlock_unlock(_lock);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Unlocked %p (%s) at %s:%d",       \
				     _lock, #_lock,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, unlocking %p (%s) at %s:%d",	\
				rc, _lock, #_lock,			\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)							\

/**
 * @brief Logging pthread mutex attribute initialization
 *
 * @param[in,out] _attr The attributes to initialize
 */
#define PTHREAD_MUTEXATTR_init(_attr)					\
	do {								\
		int rc;							\
									\
		rc = pthread_mutexattr_init(_attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init mutexattr %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, mutexattr init %p (%s) "	\
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging pthread mutex set type
 *
 * @param[in,out] _attr The attributes to update
 * @param[in]     _type The type to set
 */
#define PTHREAD_MUTEXATTR_settype(_attr, _type)				\
	do {								\
		int rc;							\
									\
		rc = pthread_mutexattr_settype(_attr, _type);		\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "pthread_mutexattr_settype %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, mutexattr settype %p (%s) "	\
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging pthread mutex attribute destruction
 *
 * @param[in,out] _attr The attributes to initialize
 */
#define PTHREAD_MUTEXATTR_destroy(_attr)				\
	do {								\
		int rc;							\
									\
		rc = pthread_mutexattr_destroy(_attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy mutexattr %p (%s) at %s:%d", \
				     _attr, #_attr,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, mutexattr destroy %p (%s) "	\
				"at %s:%d", rc, _attr, #_attr,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging mutex lock
 *
 * @param[in,out] _mtx The mutex to acquire
 */

#define PTHREAD_MUTEX_lock(_mtx)					\
	do {								\
		int rc;							\
									\
		rc = pthread_mutex_lock(_mtx);				\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Acquired mutex %p (%s) at %s:%d",	\
				     _mtx, #_mtx,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, acquiring mutex %p (%s) "	\
				"at %s:%d", rc, _mtx, #_mtx,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

static inline int PTHREAD_mutex_trylock(pthread_mutex_t *mtx,
                                        const char *mtx_name)
{
    int rc;

    rc = pthread_mutex_trylock(mtx);
    if (rc == 0) {
        LogFullDebug(COMPONENT_RW_LOCK,
                     "Acquired mutex %p (%s) at %s:%d",
                     mtx, mtx_name,
                     __FILE__, __LINE__);
    } else if (rc == EBUSY) {
        LogFullDebug(COMPONENT_RW_LOCK,
                     "Busy mutex %p (%s) at %s:%d",
                     mtx, mtx_name,
                     __FILE__, __LINE__);
    } else {
        LogCrit(COMPONENT_RW_LOCK,
                "Error %d, acquiring mutex %p (%s) at %s:%d",
                rc, mtx, mtx_name,
                __FILE__, __LINE__);
        abort();
    }

    return rc;
}

#define PTHREAD_MUTEX_trylock(_mtx)  PTHREAD_mutex_trylock(_mtx, #_mtx)

/**
 * @brief Logging mutex unlock
 *
 * @param[in,out] _mtx The mutex to relinquish
 */

#define PTHREAD_MUTEX_unlock(_mtx)					\
	do {								\
		int rc;							\
									\
		rc = pthread_mutex_unlock(_mtx);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Released mutex %p (%s) at %s:%d",	\
				     _mtx, #_mtx,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, releasing mutex %p (%s) "	\
				"at %s:%d", rc, _mtx, #_mtx,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging mutex initialization
 *
 * @param[in,out] _mtx The mutex to initialize
 * @param[in,out] _attr The attributes used while initializing the mutex
 */
#define PTHREAD_MUTEX_init(_mtx, _attr)					\
	do {								\
		int rc;							\
		pthread_mutexattr_t *attr = _attr;			\
									\
		if (attr == NULL)					\
			attr = &default_mutex_attr;			\
									\
		rc = pthread_mutex_init(_mtx, attr);			\
									\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init mutex %p (%s) at %s:%d",	\
				     _mtx, #_mtx,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Init mutex %p (%s) "		\
				"at %s:%d", rc, _mtx, #_mtx,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging mutex destroy
 *
 * @param[in,out] _mtx The mutex to destroy
 */

#define PTHREAD_MUTEX_destroy(_mtx)					\
	do {								\
		int rc;							\
									\
		rc = pthread_mutex_destroy(_mtx);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy mutex %p (%s) at %s:%d",	\
				     _mtx, #_mtx,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Destroy mutex %p (%s) "	\
				"at %s:%d", rc, _mtx, #_mtx,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging spin lock
 *
 * @param[in,out] _spin The spin lock to acquire
 */

#define PTHREAD_SPIN_lock(_spin)					\
	do {								\
		int rc;							\
									\
		rc = pthread_spin_lock(_spin);				\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Acquired spin lock %p (%s) at %s:%d", \
				     _spin, #_spin,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, acquiring spin lock %p (%s) " \
				"at %s:%d", rc, _spin, #_spin,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

#define PTHREAD_SPIN_unlock(_spin)					\
	do {								\
		int rc;							\
									\
		rc = pthread_spin_unlock(_spin);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Released spin lock %p (%s) at %s:%d", \
				     _spin, #_spin,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, releasing spin lock %p (%s) " \
				"at %s:%d", rc, _spin, #_spin,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging spin lock initialization
 *
 * @param[in,out] _spin    The spin lock to initialize
 * @param[in,out] _pshared The sharing type for the spin lock
 */
#define PTHREAD_SPIN_init(_spin, _pshared)				\
	do {								\
		int rc;							\
									\
		rc = pthread_spin_init(_spin, _pshared);		\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init spin lock %p (%s) at %s:%d",	\
				     _spin, #_spin,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Init spin lock %p (%s) "	\
				"at %s:%d", rc, _spin, #_spin,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging spin lock destroy
 *
 * @param[in,out] _spin The spin lock to destroy
 */

#define PTHREAD_SPIN_destroy(_spin)					\
	do {								\
		int rc;							\
									\
		rc = pthread_spin_destroy(_spin);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy spin lock %p (%s) at %s:%d", \
				     _spin, #_spin,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Destroy spin lock %p (%s) "	\
				"at %s:%d", rc, _spin, #_spin,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging condition variable initialization
 *
 * @param[in,out] _cond The condition variable to initialize
 * @param[in,out] _attr The attributes used while initializing the
 *			condition variable
 */
#define PTHREAD_COND_init(_cond, _attr)					\
	do {								\
		int rc;							\
									\
		rc = pthread_cond_init(_cond, _attr);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Init cond %p (%s) at %s:%d",	\
				     _cond, #_cond,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Init cond %p (%s) "		\
				"at %s:%d", rc, _cond, #_cond,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging condition variable destroy
 *
 * @param[in,out] _cond The condition variable to destroy
 */

#define PTHREAD_COND_destroy(_cond)					\
	do {								\
		int rc;							\
									\
		rc = pthread_cond_destroy(_cond);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Destroy cond %p (%s) at %s:%d",	\
				     _cond, #_cond,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Destroy cond %p (%s) "	\
				"at %s:%d", rc, _cond, #_cond,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging condtion variable wait
 *
 * @param[in,out] _cond The condition variable to wait for
 * @param[in,out] _mutex The mutex associated with the condition variable
 */

#define PTHREAD_COND_wait(_cond, _mutex)				\
	do {								\
		int rc;							\
									\
		rc = pthread_cond_wait(_cond, _mutex);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Wait cond %p (%s) at %s:%d",	\
				     _cond, #_cond,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Wait cond %p (%s) "		\
				"at %s:%d", rc, _cond, #_cond,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)

/**
 * @brief Logging condtion variable signal
 *
 * @param[in,out] _cond The condition variable to destroy
 */

#define PTHREAD_COND_signal(_cond)					\
	do {								\
		int rc;							\
									\
		rc = pthread_cond_signal(_cond);			\
		if (rc == 0) {						\
			LogFullDebug(COMPONENT_RW_LOCK,			\
				     "Wait cond %p (%s) at %s:%d",	\
				     _cond, #_cond,			\
				     __FILE__, __LINE__);		\
		} else {						\
			LogCrit(COMPONENT_RW_LOCK,			\
				"Error %d, Wait cond %p (%s) "		\
				"at %s:%d", rc, _cond, #_cond,		\
				__FILE__, __LINE__);			\
			abort();					\
		}							\
	} while (0)
