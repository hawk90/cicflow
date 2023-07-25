#ifndef CIC_BASE_CALL_ONCE_H_
#define CIC_BASE_CALL_ONCE_H_

// TODO: need comfirm
#if defined(__WIN32__)
#include <threads.h>
#elif defined(__POSIX__)
#include <pthread.h>
#endif
#include <stdatomic.h>

// #include "cicflow/base/internal/invoke.h"
#include "cicflow/base/internal/low_level_scheduling.h"
#include "cicflow/base/internal/raw_logging.h"
#include "cicflow/base/internal/scheduling_mode.h"
#include "cicflow/base/internal/spinlock_wait.h"
#include "cicflow/base/macros.h"
#include "cicflow/base/optimization.h"
#include "cicflow/base/port.h"

#define CIC_ONCE_INIT 0

typedef void (*void_func_t)(void);

enum {
  ONCE_INIT = 0,
  ONCE_RUNNING = 0x65c2937b,
  ONCE_WAITER = 0x05a308d2,
  ONCE_DONE = 221,  // RANDOM NUMBER
};

typedef struct {
  _Atomic(uint32_t) control;
} once_flag_t;

void call_once_impl(_Atomic(uint32_t) *control, SCHEDULING_MODE scheduling_mode, void_func_t *fn) {
#ifndef NDEBUG

#endif  // NDEBUG

  SPINLOCK_WAIT_TRANSITION trans[] = {
      {ONCE_INIT, ONCE_RUNNING, true}, {ONCE_RUNNING, ONCE_WAITER, false}, {ONCE_DONE, ONCE_DONE, true}};

  _Atomic(uint32_t) old_control = ONCE_INIT;
  if (atomic_compare_exchange_strong_explicit(control, &old_control, ONCE_RUNNING, memory_order_acq_rel,
                                              memory_order_acquire) ||
      spinlock_wait(control, sizeof(trans), trans, scheduling_mode) == ONCE_INIT) {
    atomic_exchange_explicit(&old_control, *control, memory_order_release);
    if (old_control == ONCE_WAITER) {
      spinlock_wake(control, true);
    }
  }
}

inline _Atomic(uint32_t) *control_word(once_flag_t *flag) {
  // return &flag->control_;
  return &flag->control;
}

void low_level_call_onece(once_flag_t *flag, void_func_t *fn) {
  _Atomic(uint32_t) *once = control_word(flag);
  uint32_t s = atomic_load_explicit(once, memory_order_acquire);
  if (CIC_PREDICT_FALSE(s != ONCE_DONE)) {
    call_once_impl(once, SCHEDULE_KERNEL_ONLY, fn);
  }
}

void call_once(once_flag_t *flag, void_func_t *fn) {
  _Atomic(uint32_t) *once = control_word(flag);
  uint32_t s = atomic_load_explicit(once, memory_order_acquire);
  if (CIC_PREDICT_FALSE(s != ONCE_DONE)) {
    call_once_impl(once, SCHEDULE_COOPERATIVE_AND_KERNEL, fn);
  }
}
#endif  // CIC_BASE_CALL_ONCE_H_
