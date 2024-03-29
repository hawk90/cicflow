#ifndef CIC_BASE_INTERNAL_UNSCALEDCYCLECLOCK_CONFIG_H_
#define CIC_BASE_INTERNAL_UNSCALEDCYCLECLOCK_CONFIG_H_

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#if defined(__i386__) || defined(__x86_64__) || defined(__aarch64__) || defined(__powerpc__) || \
    defined(__ppc__) || defined(__riscv) || defined(_M_IX86) ||                                 \
    (defined(_M_X64) && !defined(_M_ARM64EC))
#define CIC_HAVE_UNSCALED_CYCLECLOCK_IMPLEMENTATION 1
#else
#define CIC_HAVE_UNSCALED_CYCLECLOCK_IMPLEMENTATION 0
#endif

#if defined(__native_client__) || (defined(__APPLE__)) || \
    (defined(__ANDROID__) && defined(__aarch64__))
#define CIC_USE_UNSCALED_CYCLECLOCK_DEFAULT 0
#else
#define CIC_USE_UNSCALED_CYCLECLOCK_DEFAULT 1
#endif

#if !defined(CIC_USE_UNSCALED_CYCLECLOCK)
#define CIC_USE_UNSCALED_CYCLECLOCK \
  (CIC_HAVE_UNSCALED_CYCLECLOCK_IMPLEMENTATION && CIC_USE_UNSCALED_CYCLECLOCK_DEFAULT)
#endif

#if CIC_USE_UNSCALED_CYCLECLOCK
#if (defined(__i386__) || defined(__x86_64__) || defined(__riscv) || defined(_M_IX86) || \
     defined(_M_X64))
#define CIC_INTERNAL_UNSCALED_CYCLECLOCK_FREQUENCY_IS_CPU_FREQUENCY
#endif
#endif

#endif  // CIC_BASE_INTERNAL_UNSCALEDCYCLECLOCK_CONFIG_H_
