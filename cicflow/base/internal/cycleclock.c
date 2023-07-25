#include "cicflow/base/internal/cycleclock.h"

#include <stdatomic.h>
#include <time.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#include "cicflow/base/internal/unscaledcycleclock.h"

#if CIC_USE_UNSCALED_CYCLECLOCK

#define BILLION 1000000000L

int64_t cycle_clock_now() {
  uint64_t now_ns;
  struct timespec now;
  clock_gettime(CLOCK_RELATIME, &now);

  return BILLION * now.tv_sec + now.tv_nsec;
}

double cycle_clock_frequency() { return 1e9; }

#endif
