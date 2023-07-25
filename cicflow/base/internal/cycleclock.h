#ifndef CIC_BASE_INTERNAL_CYCLECLOCK_H_
#define CIC_BASE_INTERNAL_CYCLECLOCK_H_

#include <stdatomic.h>
#include <stdint.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#include "cicflow/base/internal/cycleclock_config.h"
#include "cicflow/base/internal/unscaledcycleclock.h"

typedef int64_t (*CycleClockSourceFunc)();

// -----------------------------------------------------------------------------
// CycleClock
// -----------------------------------------------------------------------------
static int64_t Now();

static double Frequency();

#if CIC_USE_UNSCALED_CYCLECLOCK
static CycleClockSourceFunc LoadCycleClockSource();

static constexpr int32_t kShift = kCycleClockShift;
static constexpr double kFrequencyScale = kCycleClockFrequencyScale;

static _Atomic(CycleClockSourceFunc) cycle_clock_source_;

static void Register(CycleClockSourceFunc source);

inline CycleClockSourceFunc LoadCycleClockSource() {
#if !defined(__x86_64__)
  if (atomic_load_explicit(cycle_clock_source_, memory_order_relaxed) == NULL) {
    return NULL;
  }
#endif  // !defined(__x86_64__)
  return atomic_load_explicit(cycle_clock_source_, memory_order_relaxed)
}

#ifndef _WIN32
inline int64_t Now() {
  CycleClockSourceFunc fn = LoadCycleClockSource();
  if (fn == NULL) {
    return UnscaledCycleClockNow() >> kShift;
  }
  return fn() >> kShift;
}
#endif

inline double Frequency() { return kFrequencyScale * UnscaledCycleClockFrequency(); }

#endif  // CIC_USE_UNSCALED_CYCLECLOCK

#endif  // CIC_BASE_INTERNAL_CYCLECLOCK_H_
