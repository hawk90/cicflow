
// This header file defines portable macros for performance optimization.

#ifndef CICFLOW_BASE_OPTIMIZATION_H_
#define CICFLOW_BASE_OPTIMIZATION_H_

#include "cicflow/base/config.h"

//
#if defined(__pnacl__)
#define CIC_BLOCK_TAIL_CALL_OPTIMIZATION() \
  if (volatile int x = 0) {                \
    (void)x;                               \
  }
#elif defined(__clang__)
// Clang will not tail call given inline volatile assembly.
#define CIC_BLOCK_TAIL_CALL_OPTIMIZATION() __asm__ __volatile__("")
#elif defined(__GNUC__)
// GCC will not tail call given inline volatile assembly.
#define CIC_BLOCK_TAIL_CALL_OPTIMIZATION() __asm__ __volatile__("")
#elif defined(_MSC_VER)
#include <intrin.h>
// The __nop() intrinsic blocks the optimisation.
#define CIC_BLOCK_TAIL_CALL_OPTIMIZATION() __nop()
#else
#define CIC_BLOCK_TAIL_CALL_OPTIMIZATION() \
  if (volatile int x = 0) {                \
    (void)x;                               \
  }
#endif

//
#if defined(__GNUC__)
// Cache line alignment
#if defined(__i386__) || defined(__x86_64__)
#define CIC_CACHELINE_SIZE 64
#elif defined(__powerpc64__)
#define CIC_CACHELINE_SIZE 128
#elif defined(__aarch64__)
#define CIC_CACHELINE_SIZE 64
#elif defined(__arm__)
#if defined(__ARM_ARCH_5T__)
#define CIC_CACHELINE_SIZE 32
#elif defined(__ARM_ARCH_7A__)
#define CIC_CACHELINE_SIZE 64
#endif
#endif
#endif

#ifndef CIC_CACHELINE_SIZE
#define CIC_CACHELINE_SIZE 64
#endif

//
#if defined(__clang__) || defined(__GNUC__)
#define CIC_CACHELINE_ALIGNED __attribute__((aligned(CIC_CACHELINE_SIZE)))
#elif defined(_MSC_VER)
#define CIC_CACHELINE_ALIGNED __declspec(align(CIC_CACHELINE_SIZE))
#else
#define CIC_CACHELINE_ALIGNED
#endif

//
#if CIC_HAVE_BUILTIN(__builtin_expect) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_PREDICT_FALSE(x) (__builtin_expect(false || (x), false))
#define CIC_PREDICT_TRUE(x) (__builtin_expect(false || (x), true))
#else
#define CIC_PREDICT_FALSE(x) (x)
#define CIC_PREDICT_TRUE(x) (x)
#endif

//
#if CIC_HAVE_BUILTIN(__builtin_trap) || (defined(__GNUC__) && !defined(__clang__))
#define CIC_INTERNAL_IMMEDIATE_ABORT_IMPL() __builtin_trap()
#else
#define CIC_INTERNAL_IMMEDIATE_ABORT_IMPL() abort()
#endif

//
#if defined(__GNUC__) || CIC_HAVE_BUILTIN(__builtin_unreachable)
#define CIC_INTERNAL_UNREACHABLE_IMPL() __builtin_unreachable()
#elif CIC_HAVE_BUILTIN(__builtin_assume)
#define CIC_INTERNAL_UNREACHABLE_IMPL() __builtin_assume(false)
#elif defined(_MSC_VER)
#define CIC_INTERNAL_UNREACHABLE_IMPL() __assume(false)
#else
#define CIC_INTERNAL_UNREACHABLE_IMPL()
#endif

#if CIC_OPTION_HARDENED == 1 && defined(NDEBUG)
#define CIC_UNREACHABLE()                \
  do {                                   \
    CIC_INTERNAL_IMMEDIATE_ABORT_IMPL(); \
    CIC_INTERNAL_UNREACHABLE_IMPL();     \
  } while (false)
#else
#define CIC_UNREACHABLE()                       \
  do {                                          \
    /* NOLINTNEXTLINE: misc-static-assert */    \
    assert(false && "CIC_UNREACHABLE reached"); \
    CIC_INTERNAL_UNREACHABLE_IMPL();            \
  } while (false)
#endif

//
#if !defined(NDEBUG)
#define CIC_ASSUME(cond) assert(cond)
#elif CIC_HAVE_BUILTIN(__builtin_assume)
#define CIC_ASSUME(cond) __builtin_assume(cond)
#elif defined(_MSC_VER)
#define CIC_ASSUME(cond) __assume(cond)
#elif defined(__GNUC__) || CIC_HAVE_BUILTIN(__builtin_unreachable)
#define CIC_ASSUME(cond)                  \
  do {                                    \
    if (!(cond)) __builtin_unreachable(); \
  } while (false)
#else
#define CIC_ASSUME(cond)       \
  do {                         \
    (void *)(false && (cond)); \
  } while (false)
#endif

//
#if defined(__GNUC__)
#define CIC_INTERNAL_UNIQUE_SMALL_NAME2(x) #x
#define CIC_INTERNAL_UNIQUE_SMALL_NAME1(x) CIC_INTERNAL_UNIQUE_SMALL_NAME2(x)
#define CIC_INTERNAL_UNIQUE_SMALL_NAME() asm(CIC_INTERNAL_UNIQUE_SMALL_NAME1(.cic.__COUNTER__))
#else
#define CIC_INTERNAL_UNIQUE_SMALL_NAME()
#endif

#endif  // CICFLOW_BASE_OPTIMIZATION_H_
