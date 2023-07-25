#ifndef CIC_BASE_INTERNAL_CYCLECLOCK_CONFIG_H_
#define CIC_BASE_INTERNAL_CYCLECLOCK_CONFIG_H_

#include <stdint.h>

#include "cicflow/base/config.h"
#include "cicflow/base/internal/unscaledcycleclock_config.h"

#if CIC_USE_UNSCALED_CYCLECLOCK
#ifdef NDEBUG
#ifdef CIC_INTERNAL_UNSCALED_CYCLECLOCK_FREQUENCY_IS_CPU_FREQUENCY
const int32_t CYCLE_CLOCK_SHIFT = 1;
#else
const int32_t CYCLE_CLOC_KSHIFT = 0;
#endif
#else   // NDEBUG
const int32_t CYCLE_CLOCK_SHIFT = 2;
#endif  // NDEBUG
const double CYCLE_CLOCK_FREQUENCY_SCALE = 1.0 / (1 << CYCLE_CLOCK_SHIFT);
#endif  //  CIC_USE_UNSCALED_CYCLECLOC

#endif  // CIC_BASE_INTERNAL_CYCLECLOCK_CONFIG_H_
