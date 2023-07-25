#include "cicflow/base/internal/spinlock_wait.h"

#include <stdatomic.h>
#include <stdint.h>

#if defined(__linux__)
#include "cicflow/base/internal/spinlock_linux.inc"
#else
// #include "cicflow/base/internal/spinlock_posix.inc"
#endif

// See spinlock_wait.h for spec.
uint32_t spinlock_wait(_Atomic(uint32_t) *w, int n, const SPINLOCK_WAIT_TRANSITION trans[],
                       SCHEDULING_MODE scheduling_mode) {
  int loop = 0;
  for (;;) {
    uint32_t v;
    // uint32_t v = w->load (memory_order_acquire);
    int i;
    for (i = 0; i != n && v != trans[i].from; i++) {
    }
    if (i == n) {
      spinlock_delay(w, v, ++loop,
                     scheduling_mode);  // no matching transition
    } else if (trans[i].to == v ||      // null transition
               atomic_compare_exchange_strong_explicit(w, &v, trans[i].to, memory_order_acquire,
                                                       memory_order_relaxed)) {
      if (trans[i].done) return v;
    }
  }
}

static _Atomic(uint64_t) delay_rand;
// Return a suggested delay in nanoseconds for iteration number "loop"
int SpinLockSuggestedDelayNS(int loop) {
  uint64_t r;
  r = 0x5deece66dLL * r + 0xb;  // numbers from nrand48()
  atomic_store_explicit(&delay_rand, r, memory_order_relaxed);

  if (loop < 0 || loop > 32) {
    loop = 32;
  }
  const int kMinDelay = 128 << 10;  // 128us
  int delay = kMinDelay << (loop / 8);
  return delay | ((delay - 1) & (int)(r));
}
