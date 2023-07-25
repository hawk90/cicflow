#ifndef CIC_BASE_THREAD_ANNOTATIONS_H_
#define CIC_BASE_THREAD_ANNOTATIONS_H_

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#include "cicflow/base/internal/thread_annotations.h"  // IWYU pragma: export

//
#if CIC_HAVE_ATTRIBUTE(guarded_by)
#define CIC_GUARDED_BY(x) __attribute__((guarded_by(x)))
#else
#define CIC_GUARDED_BY(x)
#endif

//
#if CIC_HAVE_ATTRIBUTE(pt_guarded_by)
#define CIC_PT_GUARDED_BY(x) __attribute__((pt_guarded_by(x)))
#else
#define CIC_PT_GUARDED_BY(x)
#endif

//
#if CIC_HAVE_ATTRIBUTE(acquired_after)
#define CIC_ACQUIRED_AFTER(...) __attribute__((acquired_after(__VA_ARGS__)))
#else
#define CIC_ACQUIRED_AFTER(...)
#endif

//
#if CIC_HAVE_ATTRIBUTE(acquired_before)
#define CIC_ACQUIRED_BEFORE(...) __attribute__((acquired_before(__VA_ARGS__)))
#else
#define CIC_ACQUIRED_BEFORE(...)
#endif

//
#if CIC_HAVE_ATTRIBUTE(exclusive_locks_required)
#define CIC_EXCLUSIVE_LOCKS_REQUIRED(...) __attribute__((exclusive_locks_required(__VA_ARGS__)))
#else
#define CIC_EXCLUSIVE_LOCKS_REQUIRED(...)
#endif

//
#if CIC_HAVE_ATTRIBUTE(shared_locks_required)
#define CIC_SHARED_LOCKS_REQUIRED(...) __attribute__((shared_locks_required(__VA_ARGS__)))
#else
#define CIC_SHARED_LOCKS_REQUIRED(...)
#endif

// CIC_LOCKS_EXCLUDED()
#if CIC_HAVE_ATTRIBUTE(locks_excluded)
#define CIC_LOCKS_EXCLUDED(...) __attribute__((locks_excluded(__VA_ARGS__)))
#else
#define CIC_LOCKS_EXCLUDED(...)
#endif

// CIC_LOCK_RETURNED()
#if CIC_HAVE_ATTRIBUTE(lock_returned)
#define CIC_LOCK_RETURNED(x) __attribute__((lock_returned(x)))
#else
#define CIC_LOCK_RETURNED(x)
#endif

// CIC_LOCKABLE
#if CIC_HAVE_ATTRIBUTE(lockable)
#define CIC_LOCKABLE __attribute__((lockable))
#else
#define CIC_LOCKABLE
#endif

// CIC_SCOPED_LOCKABLE
#if CIC_HAVE_ATTRIBUTE(scoped_lockable)
#define CIC_SCOPED_LOCKABLE __attribute__((scoped_lockable))
#else
#define CIC_SCOPED_LOCKABLE
#endif

// CIC_EXCLUSIVE_LOCK_FUNCTION()
#if CIC_HAVE_ATTRIBUTE(exclusive_lock_function)
#define CIC_EXCLUSIVE_LOCK_FUNCTION(...) __attribute__((exclusive_lock_function(__VA_ARGS__)))
#else
#define CIC_EXCLUSIVE_LOCK_FUNCTION(...)
#endif

// CIC_SHARED_LOCK_FUNCTION()
#if CIC_HAVE_ATTRIBUTE(shared_lock_function)
#define CIC_SHARED_LOCK_FUNCTION(...) __attribute__((shared_lock_function(__VA_ARGS__)))
#else
#define CIC_SHARED_LOCK_FUNCTION(...)
#endif

// CIC_UNLOCK_FUNCTION()
#if CIC_HAVE_ATTRIBUTE(unlock_function)
#define CIC_UNLOCK_FUNCTION(...) __attribute__((unlock_function(__VA_ARGS__)))
#else
#define CIC_UNLOCK_FUNCTION(...)
#endif

// CIC_EXCLUSIVE_TRYLOCK_FUNCTION() / CIC_SHARED_TRYLOCK_FUNCTION()
#if CIC_HAVE_ATTRIBUTE(exclusive_trylock_function)
#define CIC_EXCLUSIVE_TRYLOCK_FUNCTION(...) __attribute__((exclusive_trylock_function(__VA_ARGS__)))
#else
#define CIC_EXCLUSIVE_TRYLOCK_FUNCTION(...)
#endif

#if CIC_HAVE_ATTRIBUTE(shared_trylock_function)
#define CIC_SHARED_TRYLOCK_FUNCTION(...) __attribute__((shared_trylock_function(__VA_ARGS__)))
#else
#define CIC_SHARED_TRYLOCK_FUNCTION(...)
#endif

// CIC_ASSERT_EXCLUSIVE_LOCK() / CIC_ASSERT_SHARED_LOCK()
#if CIC_HAVE_ATTRIBUTE(assert_exclusive_lock)
#define CIC_ASSERT_EXCLUSIVE_LOCK(...) __attribute__((assert_exclusive_lock(__VA_ARGS__)))
#else
#define CIC_ASSERT_EXCLUSIVE_LOCK(...)
#endif

#if CIC_HAVE_ATTRIBUTE(assert_shared_lock)
#define CIC_ASSERT_SHARED_LOCK(...) __attribute__((assert_shared_lock(__VA_ARGS__)))
#else
#define CIC_ASSERT_SHARED_LOCK(...)
#endif

// CIC_NO_THREAD_SAFETY_ANALYSIS
#if CIC_HAVE_ATTRIBUTE(no_thread_safety_analysis)
#define CIC_NO_THREAD_SAFETY_ANALYSIS __attribute__((no_thread_safety_analysis))
#else
#define CIC_NO_THREAD_SAFETY_ANALYSIS
#endif

//------------------------------------------------------------------------------
// Tool-Supplied Annotations
//------------------------------------------------------------------------------

#define CIC_TS_UNCHECKED(x) ""

#define CIC_TS_FIXME(x) ""

#define CIC_NO_THREAD_SAFETY_ANALYSIS_FIXME CIC_NO_THREAD_SAFETY_ANALYSIS

#define CIC_GUARDED_BY_FIXME(x)

#endif  // CIC_BASE_THREAD_ANNOTATIONS_H_
