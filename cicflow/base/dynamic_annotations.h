#ifndef CIC_BASE_DYNAMIC_ANNOTATIONS_H_
#define CIC_BASE_DYNAMIC_ANNOTATIONS_H_

#include <stddef.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#ifdef __cplusplus
#include "cicflow/base/macros.h"
#endif

// TODO(rogeeff): Remove after the backward compatibility period.
#include "cicflow/base/internal/dynamic_annotations.h"  // IWYU pragma: export

// -------------------------------------------------------------------------
// Decide which features are enabled.

#ifdef CIC_HAVE_THREAD_SANITIZER

#define CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED 1
#define CIC_INTERNAL_READS_ANNOTATIONS_ENABLED 1
#define CIC_INTERNAL_WRITES_ANNOTATIONS_ENABLED 1
#define CIC_INTERNAL_ANNOTALYSIS_ENABLED 0
#define CIC_INTERNAL_READS_WRITES_ANNOTATIONS_ENABLED 1

#else

#define CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED 0
#define CIC_INTERNAL_READS_ANNOTATIONS_ENABLED 0
#define CIC_INTERNAL_WRITES_ANNOTATIONS_ENABLED 0

#if defined(__clang__)
#define CIC_INTERNAL_ANNOTALYSIS_ENABLED 1
#if !defined(SWIG)
#define CIC_INTERNAL_IGNORE_READS_ATTRIBUTE_ENABLED 1
#endif
#else
#define CIC_INTERNAL_ANNOTALYSIS_ENABLED 0
#endif

// Read/write annotations are enabled in Annotalysis mode; disabled otherwise.
#define CIC_INTERNAL_READS_WRITES_ANNOTATIONS_ENABLED CIC_INTERNAL_ANNOTALYSIS_ENABLED

#endif  // CIC_HAVE_THREAD_SANITIZER

#define CIC_INTERNAL_BEGIN_EXTERN_C  // empty
#define CIC_INTERNAL_END_EXTERN_C    // empty
#define CIC_INTERNAL_GLOBAL_SCOPED(F) F
#define CIC_INTERNAL_STATIC_INLINE

// -------------------------------------------------------------------------
// Define race annotations.

#if CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED == 1
#define CIC_ANNOTATE_BENIGN_RACE(pointer, description) \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateBenignRaceSized)  \
  (__FILE__, __LINE__, pointer, sizeof(*(pointer)), description)

#define CIC_ANNOTATE_BENIGN_RACE_SIZED(address, size, description) \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateBenignRaceSized)              \
  (__FILE__, __LINE__, address, size, description)

#define CIC_ANNOTATE_ENABLE_RACE_DETECTION(enable)        \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateEnableRaceDetection) \
  (__FILE__, __LINE__, enable)

// -------------------------------------------------------------
// Annotations useful for debugging.

#define CIC_ANNOTATE_THREAD_NAME(name) CIC_INTERNAL_GLOBAL_SCOPED(AnnotateThreadName)(__FILE__, __LINE__, name)

// -------------------------------------------------------------
// Annotations useful when implementing locks. They are not normally needed by
// modules that merely use locks. The `lock` argument is a pointer to the lock
// object.

// Report that a lock has been created at address `lock`.
#define CIC_ANNOTATE_RWLOCK_CREATE(lock) CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockCreate)(__FILE__, __LINE__, lock)

// Report that a linker initialized lock has been created at address `lock`.
#ifdef CIC_HAVE_THREAD_SANITIZER
#define CIC_ANNOTATE_RWLOCK_CREATE_STATIC(lock)          \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockCreateStatic) \
  (__FILE__, __LINE__, lock)
#else
#define CIC_ANNOTATE_RWLOCK_CREATE_STATIC(lock) CIC_ANNOTATE_RWLOCK_CREATE(lock)
#endif

// Report that the lock at address `lock` is about to be destroyed.
#define CIC_ANNOTATE_RWLOCK_DESTROY(lock) CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockDestroy)(__FILE__, __LINE__, lock)

// Report that the lock at address `lock` has been acquired.
// `is_w`=1 for writer lock, `is_w`=0 for reader lock.
#define CIC_ANNOTATE_RWLOCK_ACQUIRED(lock, is_w)     \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockAcquired) \
  (__FILE__, __LINE__, lock, is_w)

// Report that the lock at address `lock` is about to be released.
// `is_w`=1 for writer lock, `is_w`=0 for reader lock.
#define CIC_ANNOTATE_RWLOCK_RELEASED(lock, is_w)     \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateRWLockReleased) \
  (__FILE__, __LINE__, lock, is_w)

// Function prototypes of annotations provided by the compiler-based sanitizer
// implementation.
CIC_INTERNAL_BEGIN_EXTERN_C
void AnnotateRWLockCreate(const char *file, int line, const volatile void *lock);
void AnnotateRWLockCreateStatic(const char *file, int line, const volatile void *lock);
void AnnotateRWLockDestroy(const char *file, int line, const volatile void *lock);
void AnnotateRWLockAcquired(const char *file, int line, const volatile void *lock,
                            long is_w);  // NOLINT
void AnnotateRWLockReleased(const char *file, int line, const volatile void *lock,
                            long is_w);  // NOLINT
void AnnotateBenignRace(const char *file, int line, const volatile void *address, const char *description);
void AnnotateBenignRaceSized(const char *file, int line, const volatile void *address, size_t size,
                             const char *description);
void AnnotateThreadName(const char *file, int line, const char *name);
void AnnotateEnableRaceDetection(const char *file, int line, int enable);
CIC_INTERNAL_END_EXTERN_C

#else  // CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED == 0

#define CIC_ANNOTATE_RWLOCK_CREATE(lock)                            // empty
#define CIC_ANNOTATE_RWLOCK_CREATE_STATIC(lock)                     // empty
#define CIC_ANNOTATE_RWLOCK_DESTROY(lock)                           // empty
#define CIC_ANNOTATE_RWLOCK_ACQUIRED(lock, is_w)                    // empty
#define CIC_ANNOTATE_RWLOCK_RELEASED(lock, is_w)                    // empty
#define CIC_ANNOTATE_BENIGN_RACE(address, description)              // empty
#define CIC_ANNOTATE_BENIGN_RACE_SIZED(address, size, description)  // empty
#define CIC_ANNOTATE_THREAD_NAME(name)                              // empty
#define CIC_ANNOTATE_ENABLE_RACE_DETECTION(enable)                  // empty
#define CIC_ANNOTATE_BENIGN_RACE_STATIC(static_var, description)    // empty

#endif  // CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED

// -------------------------------------------------------------------------
// Define memory annotations.

#ifdef CIC_HAVE_MEMORY_SANITIZER

#include <sanitizer/msan_interface.h>

#define CIC_ANNOTATE_MEMORY_IS_INITIALIZED(address, size) __msan_unpoison(address, size)

#define CIC_ANNOTATE_MEMORY_IS_UNINITIALIZED(address, size) __msan_allocated_memory(address, size)

#else  // !defined(CIC_HAVE_MEMORY_SANITIZER)

// TODO(rogeeff): remove this branch
#ifdef CIC_HAVE_THREAD_SANITIZER
#define CIC_ANNOTATE_MEMORY_IS_INITIALIZED(address, size) \
  do {                                                    \
    (void)(address);                                      \
    (void)(size);                                         \
  } while (0)
#define CIC_ANNOTATE_MEMORY_IS_UNINITIALIZED(address, size) \
  do {                                                      \
    (void)(address);                                        \
    (void)(size);                                           \
  } while (0)
#else

#define CIC_ANNOTATE_MEMORY_IS_INITIALIZED(address, size)    // empty
#define CIC_ANNOTATE_MEMORY_IS_UNINITIALIZED(address, size)  // empty

#endif

#endif  // CIC_HAVE_MEMORY_SANITIZER

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
// Some of the symbols used in this section (e.g. AnnotateIgnoreReadsBegin) are
// defined by the compiler-based implementation, not by the Abseil
// library. Therefore they do not use CIC_INTERNAL_C_SYMBOL.

// Request the analysis tool to ignore all reads in the current thread until
// CIC_ANNOTATE_IGNORE_READS_END is called. Useful to ignore intentional racey
// reads, while still checking other reads and all writes.
// See also CIC_ANNOTATE_UNPROTECTED_READ.
#define CIC_ANNOTATE_IGNORE_READS_BEGIN()              \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateIgnoreReadsBegin) \
  (__FILE__, __LINE__)

// Stop ignoring reads.
#define CIC_ANNOTATE_IGNORE_READS_END()              \
  CIC_INTERNAL_GLOBAL_SCOPED(AnnotateIgnoreReadsEnd) \
  (__FILE__, __LINE__)

// Function prototypes of annotations provided by the compiler-based sanitizer
// implementation.
CIC_INTERNAL_BEGIN_EXTERN_C
void AnnotateIgnoreReadsBegin(const char *file, int line) CIC_INTERNAL_IGNORE_READS_BEGIN_ATTRIBUTE;
void AnnotateIgnoreReadsEnd(const char *file, int line) CIC_INTERNAL_IGNORE_READS_END_ATTRIBUTE;
CIC_INTERNAL_END_EXTERN_C

#elif defined(CIC_INTERNAL_ANNOTALYSIS_ENABLED)

// When Annotalysis is enabled without Dynamic Annotations, the use of
// static-inline functions allows the annotations to be read at compile-time,
// while still letting the compiler elide the functions from the final build.
//
// TODO(delesley) -- The exclusive lock here ignores writes as well, but
// allows IGNORE_READS_AND_WRITES to work properly.

#define CIC_ANNOTATE_IGNORE_READS_BEGIN()                                                    \
  CIC_INTERNAL_GLOBAL_SCOPED(CIC_INTERNAL_C_SYMBOL(cicflowInternalAnnotateIgnoreReadsBegin)) \
  ()

#define CIC_ANNOTATE_IGNORE_READS_END()                                                    \
  CIC_INTERNAL_GLOBAL_SCOPED(CIC_INTERNAL_C_SYMBOL(cicflowInternalAnnotateIgnoreReadsEnd)) \
  ()

#else

#define CIC_ANNOTATE_IGNORE_READS_BEGIN()  // empty
#define CIC_ANNOTATE_IGNORE_READS_END()    // empty

#endif

// -------------------------------------------------------------------------
// Define IGNORE_WRITES_BEGIN/_END annotations.

#if CIC_INTERNAL_WRITES_ANNOTATIONS_ENABLED == 1

// Similar to CIC_ANNOTATE_IGNORE_READS_BEGIN, but ignore writes instead.
#define CIC_ANNOTATE_IGNORE_WRITES_BEGIN() CIC_INTERNAL_GLOBAL_SCOPED(AnnotateIgnoreWritesBegin)(__FILE__, __LINE__)

// Stop ignoring writes.
#define CIC_ANNOTATE_IGNORE_WRITES_END() CIC_INTERNAL_GLOBAL_SCOPED(AnnotateIgnoreWritesEnd)(__FILE__, __LINE__)

// Function prototypes of annotations provided by the compiler-based sanitizer
// implementation.
CIC_INTERNAL_BEGIN_EXTERN_C
void AnnotateIgnoreWritesBegin(const char *file, int line);
void AnnotateIgnoreWritesEnd(const char *file, int line);
CIC_INTERNAL_END_EXTERN_C

#else

#define CIC_ANNOTATE_IGNORE_WRITES_BEGIN()  // empty
#define CIC_ANNOTATE_IGNORE_WRITES_END()    // empty

#endif

// -------------------------------------------------------------------------
// Define the CIC_ANNOTATE_IGNORE_READS_AND_WRITES_* annotations using the more
// primitive annotations defined above.
//
#if defined(CIC_INTERNAL_READS_WRITES_ANNOTATIONS_ENABLED)

// Start ignoring all memory accesses (both reads and writes).
#define CIC_ANNOTATE_IGNORE_READS_AND_WRITES_BEGIN() \
  do {                                               \
    CIC_ANNOTATE_IGNORE_READS_BEGIN();               \
    CIC_ANNOTATE_IGNORE_WRITES_BEGIN();              \
  } while (0)

// Stop ignoring both reads and writes.
#define CIC_ANNOTATE_IGNORE_READS_AND_WRITES_END() \
  do {                                             \
    CIC_ANNOTATE_IGNORE_WRITES_END();              \
    CIC_ANNOTATE_IGNORE_READS_END();               \
  } while (0)

#define CIC_ANNOTATE_IGNORE_READS_AND_WRITES_BEGIN()  // empty
#define CIC_ANNOTATE_IGNORE_READS_AND_WRITES_END()    // empty
#define CIC_ANNOTATE_UNPROTECTED_READ(x) (x)

// -------------------------------------------------------------------------
// Address sanitizer annotations

#ifdef CIC_HAVE_ADDRESS_SANITIZER
#include <sanitizer/common_interface_defs.h>

#define CIC_ANNOTATE_CONTIGUOUS_CONTAINER(beg, end, old_mid, new_mid) \
  __sanitizer_annotate_contiguous_container(beg, end, old_mid, new_mid)
#define CIC_ADDRESS_SANITIZER_REDZONE(name) \
  struct {                                  \
    alignas(8) char x[8];                   \
  } name

#else

#define CIC_ANNOTATE_CONTIGUOUS_CONTAINER(beg, end, old_mid, new_mid)  // empty
#define CIC_ADDRESS_SANITIZER_REDZONE(name) static_assert(true, "")

#endif  // CIC_HAVE_ADDRESS_SANITIZER

// -------------------------------------------------------------------------
// Undefine the macros intended only for this file.

#undef CIC_INTERNAL_RACE_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_READS_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_WRITES_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_ANNOTALYSIS_ENABLED
#undef CIC_INTERNAL_READS_WRITES_ANNOTATIONS_ENABLED
#undef CIC_INTERNAL_BEGIN_EXTERN_C
#undef CIC_INTERNAL_END_EXTERN_C
#undef CIC_INTERNAL_STATIC_INLINE

#endif  // CIC_BASE_DYNAMIC_ANNOTATIONS_H_
#endif
