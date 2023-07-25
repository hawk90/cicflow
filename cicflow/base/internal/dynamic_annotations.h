#ifndef CIC_BASE_INTERNAL_DYNAMIC_ANNOTATIONS_H_
#define CIC_BASE_INTERNAL_DYNAMIC_ANNOTATIONS_H_

#include <stddef.h>

#include "cicflow/base/config.h"

// -------------------------------------------------------------------------
// Decide which features are enabled

#ifndef DYNAMIC_ANNOTATIONS_ENABLED
#define DYNAMIC_ANNOTATIONS_ENABLED 0
#endif

#if defined(__clang__) && !defined(SWIG)
#define CIC_INTERNAL_IGNORE_READS_ATTRIBUTE_ENABLED 1
#endif

#if DYNAMIC_ANNOTATIONS_ENABLED != 0

#define CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED 1
#define CIC_INTERNAL_READS_ANNOTATIONS_ENABLED 1
#define CIC_INTERNAL_WRITES_ANNOTATIONS_ENABLED 1
#define CIC_INTERNAL_ANNOTALYSIS_ENABLED 0
#define CIC_INTERNAL_READS_WRITES_ANNOTATIONS_ENABLED 1

#else

#define CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED 0
#define CIC_INTERNAL_READS_ANNOTATIONS_ENABLED 0
#define CIC_INTERNAL_WRITES_ANNOTATIONS_ENABLED 0

#define CIC_INTERNAL_ANNOTALYSIS_ENABLED defined(CIC_INTERNAL_IGNORE_READS_ATTRIBUTE_ENABLED)
#define CIC_INTERNAL_READS_WRITES_ANNOTATIONS_ENABLED CIC_INTERNAL_ANNOTALYSIS_ENABLED
#endif

#if defined(CIC_HAVE_MEMORY_SANITIZER) && !defined(__native_client__)
#define CIC_INTERNAL_MEMORY_ANNOTATIONS_ENABLED 1
#endif

#ifndef CIC_INTERNAL_MEMORY_ANNOTATIONS_ENABLED
#define CIC_INTERNAL_MEMORY_ANNOTATIONS_ENABLED 0
#endif

#define CIC_INTERNAL_BEGIN_EXTERN_C  // empty
#define CIC_INTERNAL_END_EXTERN_C    // empty
#define CIC_INTERNAL_GLOBAL_SCOPED(F) F
#define CIC_INTERNAL_STATIC_INLINE

// Define race annotations.

#if CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED == 1

#define ANNOTATE_BENIGN_RACE(pointer, description)    \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateBenignRaceSized) \
  (__FILE__, __LINE__, pointer, sizeof(*(pointer)), description)

#define ANNOTATE_BENIGN_RACE_SIZED(address, size, description) \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateBenignRaceSized)          \
  (__FILE__, __LINE__, address, size, description)

#define ANNOTATE_ENABLE_RACE_DETECTION(enable)            \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateEnableRaceDetection) \
  (__FILE__, __LINE__, enable)

// -------------------------------------------------------------
// Annotations useful for debugging.

#define ANNOTATE_THREAD_NAME(name) CIC_INTERNAL_GLOBAL_SCOPED(AnnotateThreadName)(__FILE__, __LINE__, name)

#define ANNOTATE_RWLOCK_CREATE(lock) CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockCreate)(__FILE__, __LINE__, lock)

#ifdef CIC_HAVE_THREAD_SANITIZER
#define ANNOTATE_RWLOCK_CREATE_STATIC(lock)              \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockCreateStatic) \
  (__FILE__, __LINE__, lock)
#else
#define ANNOTATE_RWLOCK_CREATE_STATIC(lock) ANNOTATE_RWLOCK_CREATE(lock)
#endif

// Report that the lock at address `lock` is about to be destroyed.
#define ANNOTATE_RWLOCK_DESTROY(lock) CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockDestroy)(__FILE__, __LINE__, lock)

// Report that the lock at address `lock` has been acquired.
// `is_w`=1 for writer lock, `is_w`=0 for reader lock.
#define ANNOTATE_RWLOCK_ACQUIRED(lock, is_w)         \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockAcquired) \
  (__FILE__, __LINE__, lock, is_w)

// Report that the lock at address `lock` is about to be released.
// `is_w`=1 for writer lock, `is_w`=0 for reader lock.
#define ANNOTATE_RWLOCK_RELEASED(lock, is_w)         \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockReleased) \
  (__FILE__, __LINE__, lock, is_w)

// Apply ANNOTATE_BENIGN_RACE_SIZED to a static variable `static_var`.
#define ANNOTATE_BENIGN_RACE_STATIC(static_var, description)                                     \
  namespace {                                                                                    \
  class static_var##_annotator {                                                                 \
   public:                                                                                       \
    static_var##_annotator() {                                                                   \
      ANNOTATE_BENIGN_RACE_SIZED(&static_var, sizeof(static_var), #static_var ": " description); \
    }                                                                                            \
  };                                                                                             \
  static static_var##_annotator the##static_var##_annotator;                                     \
  }  // namespace

#else  // CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED == 0

#define ANNOTATE_RWLOCK_CREATE(lock)                            // empty
#define ANNOTATE_RWLOCK_CREATE_STATIC(lock)                     // empty
#define ANNOTATE_RWLOCK_DESTROY(lock)                           // empty
#define ANNOTATE_RWLOCK_ACQUIRED(lock, is_w)                    // empty
#define ANNOTATE_RWLOCK_RELEASED(lock, is_w)                    // empty
#define ANNOTATE_BENIGN_RACE(address, description)              // empty
#define ANNOTATE_BENIGN_RACE_SIZED(address, size, description)  // empty
#define ANNOTATE_THREAD_NAME(name)                              // empty
#define ANNOTATE_ENABLE_RACE_DETECTION(enable)                  // empty
#define ANNOTATE_BENIGN_RACE_STATIC(static_var, description)    // empty

#endif  // CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED

// -------------------------------------------------------------------------
// Define memory annotations.

#if CIC_INTERNAL_MEMORY_ANNOTATIONS_ENABLED == 1

#include <sanitizer/msan_interface.h>

#define ANNOTATE_MEMORY_IS_INITIALIZED(address, size) __msan_unpoison(address, size)

#define ANNOTATE_MEMORY_IS_UNINITIALIZED(address, size) __msan_allocated_memory(address, size)

#else  // CIC_INTERNAL_MEMORY_ANNOTATIONS_ENABLED == 0

#if DYNAMIC_ANNOTATIONS_ENABLED == 1
#define ANNOTATE_MEMORY_IS_INITIALIZED(address, size) \
  do {                                                \
    (void)(address);                                  \
    (void)(size);                                     \
  } while (0)
#define ANNOTATE_MEMORY_IS_UNINITIALIZED(address, size) \
  do {                                                  \
    (void)(address);                                    \
    (void)(size);                                       \
  } while (0)
#else
#define ANNOTATE_MEMORY_IS_INITIALIZED(address, size)    // empty
#define ANNOTATE_MEMORY_IS_UNINITIALIZED(address, size)  // empty
#endif

#endif  // CIC_INTERNAL_MEMORY_ANNOTATIONS_ENABLED

// -------------------------------------------------------------------------
// Define IGNORE_READS_BEGIN/_END attributes.

#if defined(CIC_INTERNAL_IGNORE_READS_ATTRIBUTE_ENABLED)

#define CIC_INTERNAL_IGNORE_READS_BEGIN_ATTRIBUTE __attribute((exclusive_lock_function("*")))
#define CIC_INTERNAL_IGNORE_READS_END_ATTRIBUTE __attribute((unlock_function("*")))

#else  // !defined(CIC_INTERNAL_IGNORE_READS_ATTRIBUTE_ENABLED)

#define CIC_INTERNAL_IGNORE_READS_BEGIN_ATTRIBUTE  // empty
#define CIC_INTERNAL_IGNORE_READS_END_ATTRIBUTE    // empty

#endif  // defined(CIC_INTERNAL_IGNORE_READS_ATTRIBUTE_ENABLED)

// -------------------------------------------------------------------------
// Define IGNORE_READS_BEGIN/_END annotations.

#if CIC_INTERNAL_READS_ANNOTATIONS_ENABLED == 1

// Request the analysis tool to ignore all reads in the current thread until
// ANNOTATE_IGNORE_READS_END is called. Useful to ignore intentional racey
// reads, while still checking other reads and all writes.
// See also ANNOTATE_UNPROTECTED_READ.
#define ANNOTATE_IGNORE_READS_BEGIN() CIC_INTERNAL_GLOBAL_SCOPED(AnnotateIgnoreReadsBegin)(__FILE__, __LINE__)

// Stop ignoring reads.
#define ANNOTATE_IGNORE_READS_END() CIC_INTERNAL_GLOBAL_SCOPED(AnnotateIgnoreReadsEnd)(__FILE__, __LINE__)

#elif defined(CIC_INTERNAL_ANNOTALYSIS_ENABLED)

// When Annotalysis is enabled without Dynamic Annotations, the use of
// static-inline functions allows the annotations to be read at compile-time,
// while still letting the compiler elide the functions from the final build.
//
// TODO(delesley) -- The exclusive lock here ignores writes as well, but
// allows IGNORE_READS_AND_WRITES to work properly.

#define ANNOTATE_IGNORE_READS_BEGIN() CIC_INTERNAL_GLOBAL_SCOPED(cicflowInternalAnnotateIgnoreReadsBegin)()

#define ANNOTATE_IGNORE_READS_END() CIC_INTERNAL_GLOBAL_SCOPED(cicflowInternalAnnotateIgnoreReadsEnd)()

#else

#define ANNOTATE_IGNORE_READS_BEGIN()  // empty
#define ANNOTATE_IGNORE_READS_END()    // empty

#endif

// -------------------------------------------------------------------------
// Define IGNORE_WRITES_BEGIN/_END annotations.

#if CIC_INTERNAL_WRITES_ANNOTATIONS_ENABLED == 1

// Similar to ANNOTATE_IGNORE_READS_BEGIN, but ignore writes instead.
#define ANNOTATE_IGNORE_WRITES_BEGIN() CIC_INTERNAL_GLOBAL_SCOPED(AnnotateIgnoreWritesBegin)(__FILE__, __LINE__)

// Stop ignoring writes.
#define ANNOTATE_IGNORE_WRITES_END() CIC_INTERNAL_GLOBAL_SCOPED(AnnotateIgnoreWritesEnd)(__FILE__, __LINE__)

#else

#define ANNOTATE_IGNORE_WRITES_BEGIN()  // empty
#define ANNOTATE_IGNORE_WRITES_END()    // empty

#endif

// -------------------------------------------------------------------------
// Define the ANNOTATE_IGNORE_READS_AND_WRITES_* annotations using the more
// primitive annotations defined above.
#if defined(CIC_INTERNAL_READS_WRITES_ANNOTATIONS_ENABLED)

// Start ignoring all memory accesses (both reads and writes).
#define ANNOTATE_IGNORE_READS_AND_WRITES_BEGIN() \
  do {                                           \
    ANNOTATE_IGNORE_READS_BEGIN();               \
    ANNOTATE_IGNORE_WRITES_BEGIN();              \
  } while (0)

// Stop ignoring both reads and writes.
#define ANNOTATE_IGNORE_READS_AND_WRITES_END() \
  do {                                         \
    ANNOTATE_IGNORE_WRITES_END();              \
    ANNOTATE_IGNORE_READS_END();               \
  } while (0)

#else

#define ANNOTATE_IGNORE_READS_AND_WRITES_BEGIN()  // empty
#define ANNOTATE_IGNORE_READS_AND_WRITES_END()    // empty
#define ANNOTATE_UNPROTECTED_READ(x) (x)

#endif

// -------------------------------------------------------------------------
// Address sanitizer annotations

#ifdef CIC_HAVE_ADDRESS_SANITIZER
#include <sanitizer/common_interface_defs.h>

#define ANNOTATE_CONTIGUOUS_CONTAINER(beg, end, old_mid, new_mid) \
  __sanitizer_annotate_contiguous_container(beg, end, old_mid, new_mid)
#define ADDRESS_SANITIZER_REDZONE(name)    \
  struct {                                 \
    char x[8] __attribute__((aligned(8))); \
  } name

#else

#define ANNOTATE_CONTIGUOUS_CONTAINER(beg, end, old_mid, new_mid)
#define ADDRESS_SANITIZER_REDZONE(name) static_assert(true, "")

#endif  // CIC_HAVE_ADDRESS_SANITIZER

// -------------------------------------------------------------------------
// Undefine the macros intended only for this file.

#undef CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_MEMORY_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_READS_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_WRITES_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_ANNOTALYSIS_ENABLED
#undef CIC_INTERNAL_READS_WRITES_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_BEGIN_EXTERN_C
#undef CIC_INTERNAL_END_EXTERN_C
#undef CIC_INTERNAL_STATIC_INLINE

#endif  // CIC_BASE_INTERNAL_DYNAMIC_ANNOTATIONS_H_
