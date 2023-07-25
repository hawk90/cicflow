#ifndef CIC_BASE_INTERNAL_TSAN_MUTEX_INTERFACE_H_
#define CIC_BASE_INTERNAL_TSAN_MUTEX_INTERFACE_H_

#include "cicflow/base/config.h"

#ifdef CIC_INTERNAL_HAVE_TSAN_INTERFACE
#error "CIC_INTERNAL_HAVE_TSAN_INTERFACE cannot be directly set."
#endif

#if defined(CIC_HAVE_THREAD_SANITIZER) && defined(__has_include)
#if __has_include(<sanitizer/tsan_interface.h>)
#define CIC_INTERNAL_HAVE_TSAN_INTERFACE 1
#endif
#endif

#ifdef CIC_INTERNAL_HAVE_TSAN_INTERFACE
#include <sanitizer/tsan_interface.h>

#define CIC_TSAN_MUTEX_CREATE __tsan_mutex_create
#define CIC_TSAN_MUTEX_DESTROY __tsan_mutex_destroy
#define CIC_TSAN_MUTEX_PRE_LOCK __tsan_mutex_pre_lock
#define CIC_TSAN_MUTEX_POST_LOCK __tsan_mutex_post_lock
#define CIC_TSAN_MUTEX_PRE_UNLOCK __tsan_mutex_pre_unlock
#define CIC_TSAN_MUTEX_POST_UNLOCK __tsan_mutex_post_unlock
#define CIC_TSAN_MUTEX_PRE_SIGNAL __tsan_mutex_pre_signal
#define CIC_TSAN_MUTEX_POST_SIGNAL __tsan_mutex_post_signal
#define CIC_TSAN_MUTEX_PRE_DIVERT __tsan_mutex_pre_divert
#define CIC_TSAN_MUTEX_POST_DIVERT __tsan_mutex_post_divert

#else

#define CIC_TSAN_MUTEX_CREATE(...)
#define CIC_TSAN_MUTEX_DESTROY(...)
#define CIC_TSAN_MUTEX_PRE_LOCK(...)
#define CIC_TSAN_MUTEX_POST_LOCK(...)
#define CIC_TSAN_MUTEX_PRE_UNLOCK(...)
#define CIC_TSAN_MUTEX_POST_UNLOCK(...)
#define CIC_TSAN_MUTEX_PRE_SIGNAL(...)
#define CIC_TSAN_MUTEX_POST_SIGNAL(...)
#define CIC_TSAN_MUTEX_PRE_DIVERT(...)
#define CIC_TSAN_MUTEX_POST_DIVERT(...)

#endif

#endif  // CIC_BASE_INTERNAL_TSAN_MUTEX_INTERFACE_H_
