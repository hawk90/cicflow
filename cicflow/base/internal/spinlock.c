#include "cicflow/base/internal/spinlock.h"

#include <limits.h>
#include <stdatomic.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/call_once.h"
#include "cicflow/base/config.h"
#include "cicflow/base/internal/atomic_hook.h"
#include "cicflow/base/internal/cycleclock.h"
#include "cicflow/base/internal/spinlock_wait.h"
#include "cicflow/base/internal/sysinfo.h" /* For NumCPUs() */

typedef void (*profile)(const void *lock, int64_t wait_cycles);
_Atomic profile submit_profile_data;

void register_spinlock_profiler(profile func) { atomic_store(&submit_profile_data, func); }

uint32_t spin_loop() {
  static once_flag_t init_adaptive_spin_count;
  static int adaptive_spin_count = 0;
  // low_level_call_once();

  int c = adaptive_spin_count;
  uint32_t lock_value;

  return lock_value;
}
