#ifndef CIC_BASE_INTERNAL_UNSCALEDCYCLECLOCK_H_
#define CIC_BASE_INTERNAL_UNSCALEDCYCLECLOCK_H_

#include <stdint.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#include "cicflow/base/config.h"
#include "cicflow/base/internal/unscaledcycleclock_config.h"

#if CIC_USE_UNSCALED_CYCLECLOCK

int64_t UnscaledCycleClockNow();
double UnscaledCycleClockFrequency();

#if defined(__x86_64__)

inline int64_t UnscaledCycleClockNow() {
  uint64_t low, high;
  __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
  return (int64_t)((high << 32) | low);
}

#endif

#endif  // CIC_USE_UNSCALED_CYCLECLOCK

#endif  // CIC_BASE_INTERNAL_UNSCALEDCYCLECLOCK_H_
