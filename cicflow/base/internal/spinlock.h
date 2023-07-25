#ifndef CIC_BASE_INTERNAL_SPINLOCK_H_
#define CIC_BASE_INTERNAL_SPINLOCK_H_

#include <stdatomic.h>
#include <stdint.h>

#include "cicflow/base/attributes.h"
// #include "cicflow/base/const_init.h"
// #include "cicflow/base/dynamic_annotations.h"
#include "cicflow/base/internal/low_level_scheduling.h"
#include "cicflow/base/internal/raw_logging.h"
#include "cicflow/base/internal/scheduling_mode.h"
#include "cicflow/base/internal/tsan_mutex_interface.h"
// #include "cicflow/base/thread_annotations.h"

uint32_t SPINLOCK_HELD = 1;
uint32_t SPINLOCK_COOPERATIVE = 1 << 1;
uint32_t SPINLOCK_DISABLE_SCHEDULING = 1 << 2;
uint32_t SPINLOCK_SLEEPER = 1 << 3;

typedef struct {
  _Atomic(uint32_t) lockword;
} SPIN_LOCK;

static const bool is_cooperative(SCHEDULING_MODE scheduling_mode) {
  return scheduling_mode == SCHEDULE_COOPERATIVE_AND_KERNEL;
}

inline uint32_t try_lock_internal(SPIN_LOCK *spin_lock, uint32_t lock_value, uint32_t wait_cycles) {
  if ((lock_value & SPINLOCK_HELD) != 0) {
    return lock_value;
  }

  uint32_t sched_disabled_bit = 0;
  if ((lock_value & SPINLOCK_COOPERATIVE) == 0) {
    // if (scheduling_guard->disable_rescheduling ())
    if (false) sched_disabled_bit = SPINLOCK_DISABLE_SCHEDULING;
  }

  if (!atomic_compare_exchange_strong_explicit(&spin_lock->lockword, &lock_value,
                                               SPINLOCK_HELD | lock_value | wait_cycles | sched_disabled_bit,
                                               memory_order_acq_rel, memory_order_acquire)) {
    // scheduling_guard->enable_rescheduling (sched_disabled_bit != 0);
  }
  return lock_value;
}

inline bool try_lock_impl(SPIN_LOCK *spin_lock) {
  uint32_t lock_value = atomic_load_explicit(&spin_lock->lockword, memory_order_acquire);
  return (try_lock_internal(spin_lock, lock_value, 0) & SPINLOCK_HELD) == 0;
}

typedef struct {
  SPIN_LOCK *lock;
} SPIN_LOCK_HOLDER;

void register_spinlock_profiler(void (*fn)(const void *lock, int64_t wait_cycles));

#endif  // CIC_BASE_INTERNAL_SPINLOCK_H_
