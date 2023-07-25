#ifndef CICFLOW_BASE_CONFIG_H_
#define CICFLOW_BASE_CONFIG_H_

#include <limits.h>
#include <stddef.h>

#include "cicflow/base/options.h"
#include "cicflow/base/policy_checks.h"

#define CIC_INTERNAL_DO_TOKEN_STR #x
#define CIC_INTERNAL_TOKEN_STR(x) CIC_INTERNAL_DO_TOKEN_STR(x)

// -----------------------------------------------------------------------------
// Compiler Feature
//

// symbols:
//  GCC     __GNUC__
//  Clang   __clang__

#ifdef __has_builtin
#define CIC_HAVE_BUILTIN(x) __has_builtin(x)
#else
#define CIC_HAVE_BUILTIN(x) 0
#endif

#ifdef __has_feature
#define CIC_HAVE_FEATURE(f) __has_feature(f)
#else
#define CIC_HAVE_FEATURE(f) 0
#endif

#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#define CIC_INTERNAL_HAVE_MIN_GNUC_VERSION(x, y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
#else
#define CIC_INTERNAL_HAVE_MIN_GNUC_VERSION(x, y) 0
#endif

#if defined(__clang__) && defined(__clang_major__) && defined(__clang_minor__)
#define CIC_INTERNAL_HAVE_MIN_CLANG_VERSION(x, y) \
  (__clang_major__ > (x) || __clang_major__ == (x) && __clang_minor__ >= (y))
#else
#define CIC_INTERNAL_HAVE_MIN_CLANG_VERSION(x, y) 0
#endif

//
#ifdef CIC_HAVE_TLS
#error CIC_HAVE_TLS cannot be directly set
#elif (defined(__linux__) || defined(__ASYLO__)) && (defined(__clang__) || defined(_GLIBCXX_HAVE_TLS))
#define CIC_HAVE_TLS 1
#endif

//
#ifdef CIC_HAVE_THREAD_LOCAL
#error CIC_HAVE_THREAD_LOCAL cannot be directly set
#elif defined(__APPLE__)
#if CIC_HAVE_FEATURE(c_thread_local) && !(__IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_9_0)
#define CIC_HAVE_THREAD_LOCAL 1
#endif
#else  // !defined(__APPLE__)
#define CIC_HAVE_THREAD_LOCAL 1
#endif

//
#ifdef CIC_HAVE_INTRINSIC_INT128
#error CIC_HAVE_INTRINSIC_INT128 cannot be directly set
#elif defined(__SIZEOF_INT128__)
#if (defined(__clang__) && !defined(_WIN32)) || (defined(__CUDACC__) && __CUDACC_VER_MAJOR__ >= 9) || \
    (defined(__GNUC__) && !defined(__clang__) && !defined(__CUDACC__))
#define CIC_HAVE_INTRINSIC_INT128 1
#elif defined(__CUDACC__)
#if __CUDACC_VER__ >= 70000
#define CIC_HAVE_INTRINSIC_INT128 1
#endif  // __CUDACC_VER__ >= 70000
#endif  // defined(__CUDACC__)
#endif  // CIC_HAVE_INTRINSIC_INT128

// -----------------------------------------------------------------------------
// Platform Feature
//

// symbols:
//
//  Linux and Linux-derived     __linux__
//  Linux (non-Android)         __linux__ ** !__ANDROID__
//  Darwin (macOS iOS)          __APPLE__
//  Windows                     _WIN32

//
#ifdef CIC_HAVE_MMAP
#error CIC_HAVE_MMAP cannot be directly set
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(_AIX) || defined(__ros__) ||         \
    defined(__native_client__) || defined(__asmjs__) || defined(__wasm__) || defined(__Fuchsia__) || defined(__sun) || \
    defined(__ASYLO__) || defined(__myriad2__) || defined(__HAIKU__) || defined(__OpenBSD__) || defined(__NetBSD__) || \
    defined(__QNX__)
#define CIC_HAVE_MMAP 1
#endif

//
#ifdef CIC_HAVE_PTHREAD_GETSCHEDPARAM
#error CIC_HAVE_PTHREAD_GETSCHEDPARAM cannot be directly set
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(_AIX) || defined(__ros__) || \
    defined(__OpenBSD__) || defined(__NetBSD__)
#define CIC_HAVE_PTHREAD_GETSCHEDPARAM 1
#endif

//
#ifdef CIC_HAVE_SCHED_GETCPU
#error CIC_HAVE_SCHED_GETCPU cannot be directly set
#elif defined(__linux__)
#define CIC_HAVE_SCHED_GETCPU 1
#endif

//
#ifdef CIC_HAVE_SCHED_YIELD
#error CIC_HAVE_SCHED_YIELD cannot be directly set
#elif defined(__linux__) || defined(__ros__) || defined(__native_client__)
#define CIC_HAVE_SCHED_YIELD 1
#endif

//
#ifdef CIC_HAVE_SEMAPHORE_H
#error CIC_HAVE_SEMAPHORE_H cannot be directly set
#elif defined(__linux__) || defined(__ros__)
#define CIC_HAVE_SEMAPHORE_H 1
#endif

//
#ifdef CIC_HAVE_ALARM
#error CIC_HAVE_ALARM cannot be directly set
#elif defined(__GOOGLE_GRTE_VERSION__)
#define CIC_HAVE_ALARM 1
#elif defined(__GLIBC__)
#define CIC_HAVE_ALARM 1
#elif defined(_MSC_VER)
#elif defined(__MINGW32__)
#elif defined(__EMSCRIPTEN__)
#elif defined(__Fuchsia__)
#elif defined(__native_client__)
#else
// other standard libraries
#define CIC_HAVE_ALARM 1
#endif

//
#if defined(CIC_IS_BIG_ENDIAN)
#error "CIC_IS_BIG_ENDIAN cannot be directly set."
#endif
#if defined(CIC_IS_LITTLE_ENDIAN)
#error "CIC_IS_LITTLE_ENDIAN cannot be directly set."
#endif

#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define CIC_IS_LITTLE_ENDIAN 1
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define CIC_IS_BIG_ENDIAN 1
#elif defined(_WIN32)
#define CIC_IS_LITTLE_ENDIAN 1
#else
#error "cicflow endian detection needs to be set up for your compiler"
#endif

//
#ifdef CIC_HAVE_MEMORY_SANITIZER
#error "CIC_HAVE_MEMORY_SANITIZER cannot be directly set."
#elif !defined(__native_client__) && CIC_HAVE_FEATURE(memory_sanitizer)
#define CIC_HAVE_MEMORY_SANITIZER 1
#endif

//
#ifdef CIC_HAVE_THREAD_SANITIZER
#error "CIC_HAVE_THREAD_SANITIZER cannot be directly set."
#elif defined(__SANITIZE_THREAD__)
#define CIC_HAVE_THREAD_SANITIZER 1
#elif CIC_HAVE_FEATURE(thread_sanitizer)
#define CIC_HAVE_THREAD_SANITIZER 1
#endif

//
#ifdef CIC_HAVE_ADDRESS_SANITIZER
#error "CIC_HAVE_ADDRESS_SANITIZER cannot be directly set."
#elif defined(__SANITIZE_ADDRESS__)
#define CIC_HAVE_ADDRESS_SANITIZER 1
#elif CIC_HAVE_FEATURE(address_sanitizer)
#define CIC_HAVE_ADDRESS_SANITIZER 1
#endif

// Maybe I will not use this feature
#ifdef CIC_HAVE_HWADDRESS_SANITIZER
#error "CIC_HAVE_HWADDRESS_SANITIZER cannot be directly set."
#elif defined(__SANITIZE_HWADDRESS__)
#define CIC_HAVE_HWADDRESS_SANITIZER 1
#elif CIC_HAVE_FEATURE(hwaddress_sanitizer)
#define CIC_HAVE_HWADDRESS_SANITIZER 1
#endif

//
#ifdef CIC_HAVE_LEAK_SANITIZER
#error "CIC_HAVE_LEAK_SANITIZER cannot be directly set."
#elif defined(LEAK_SANITIZER)
#define CIC_HAVE_LEAK_SANITIZER 1
#elif CIC_HAVE_FEATURE(leak_sanitizer)
#define CIC_HAVE_LEAK_SANITIZER 1
#elif defined(CIC_HAVE_ADDRESS_SANITIZER)
#define CIC_HAVE_LEAK_SANITIZER 1
#endif

//
#ifdef CIC_INTERNAL_HAVE_SSE
#error CIC_INTERNAL_HAVE_SSE cannot be directly set
#elif defined(__SSE__)
#define CIC_INTERNAL_HAVE_SSE 1
#elif defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
#define CIC_INTERNAL_HAVE_SSE 1
#endif

//
#ifdef CIC_INTERNAL_HAVE_SSE2
#error CIC_INTERNAL_HAVE_SSE2 cannot be directly set
#elif defined(__SSE2__)
#define CIC_INTERNAL_HAVE_SSE2 1
#elif defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#define CIC_INTERNAL_HAVE_SSE2 1
#endif

//
#ifdef CIC_INTERNAL_HAVE_SSSE3
#error CIC_INTERNAL_HAVE_SSSE3 cannot be directly set
#elif defined(__SSSE3__)
#define CIC_INTERNAL_HAVE_SSSE3 1
#endif

//
#ifdef CIC_INTERNAL_HAVE_ARM_NEON
#error CIC_INTERNAL_HAVE_ARM_NEON cannot be directly set
#elif defined(__ARM_NEON) && !defined(__CUDA_ARCH__)
#define CIC_INTERNAL_HAVE_ARM_NEON 1
#endif

#endif  // CICFLOW_BASE_CONFIG_H_
