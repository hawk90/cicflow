#include "cicflow/base/internal/unscaledcycleclock.h"

#if CICF_USE_UNSCALED_CYCLECLOCK

#if defined(_WIN32)
#include <intrin.h>
#endif  // _WIN32

#if defined(__powerpc__) || defined(__ppc__)
#ifdef __GLIBC__
#include <sys/platform/ppc.h>
#elif defined(__FreeBSD__)
// clang-format off
#include <sys/types.h>
#include <sys/sysctl.h>
// clang-format on

#include "cicflow/base/call_once.h"
#endif  // __GLIBC__
#endif  // __powerpc__ || __ppc__

#include "cicflow/base/internal/sysinfo.h"

#if defined(__i386__)
int64_t unscaled_cycle_clock() {
  int64_t ret;
  __asm__ volatile("rdtsc" : "=A"(ret));
  return ret;
}

double unscaled_cycle_clock_frequency() { return nominal_cpu_frequency(); }

#elif defined(__x86_64__)

double unscaled_cycle_clock_frequency() { return nominal_cpu_frequency(); }

#elif defined(__powerpc__) || defined(__ppc__)

int64_t unscaled_cycle_clock_now() {
#ifdef __GLIBC__
  return __ppc_get_timebase();
#else
#ifdef __powerpc64__
  int64_t tbr;
  asm volatile("mfspr %0, 268" : "=r"(tbr));
  return tbr;
#else
  int32_t tbu, tbl, tmp;
  asm volatile(
      "0:\n"
      "mftbu %[hi32]\n"
      "mftb %[lo32]\n"
      "mftbu %[tmp]\n"
      "cmpw %[tmp],%[hi32]\n"
      "bne $-16\n"  // Retry on failure.
      : [hi32] "=r"(tbu), [lo32] "=r"(tbl), [tmp] "=r"(tmp));
  return (static_cast<int64_t>(tbu) << 32) | tbl;
#endif
#endif
}

double unscaled_cycle_clock_frequency() {
#ifdef __GLIBC__
  return __ppc_get_timebase_freq();
#elif defined(_AIX)
  // This is the same constant value as returned by
  // __ppc_get_timebase_freq().
  return static_cast<double>(512000000);
#elif defined(__FreeBSD__)
  static once_flag init_timebase_frequency_once;
  static double timebase_frequency = 0.0;

  // TODO: implement LowLevelCallOnce
  // base_internal::LowLevelCallOnce (&init_timebase_frequency_once, [&] () {
  //   size_t length = sizeof (timebase_frequency);
  //   sysctlbyname ("kern.timecounter.tc.timebase.frequency",
  //                 &timebase_frequency, &length, nullptr, 0);
  // });
  return timebase_frequency;
#else
#error Must implement unscaled_cycle_clock_frequency()
#endif
}

#elif defined(__aarch64__)

// System timer of ARMv8 runs at a different frequency than the CPU's.
// The frequency is fixed, typically in the range 1-50MHz.  It can be
// read at CNTFRQ special register.  We assume the OS has set up
// the virtual timer properly.
int64_t unscaled_cycle_clock_now() {
  int64_t virtual_timer_value;
  asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
  return virtual_timer_value;
}

double unscaled_cycle_clock_frequency() {
  uint64_t aarch64_timer_frequency;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(aarch64_timer_frequency));
  return aarch64_timer_frequency;
}

#elif defined(__riscv)

int64_t unscaled_cycle_clock_now() {
  int64_t virtual_timer_value;
  asm volatile("rdcycle %0" : "=r"(virtual_timer_value));
  return virtual_timer_value;
}

double unscaled_cycle_clock_frequency() { return nominal_cpu_frequency(); }

#elif defined(_M_IX86) || defined(_M_X64)

#pragma intrinsic(__rdtsc)

int64_t unscaled_cycle_clock_now() { return __rdtsc(); }

double unscaled_cycle_clock_frequency() { return nominal_cpu_frequency(); }

#endif

#endif  // cicflow_USE_UNSCALED_CYCLECLOCK
