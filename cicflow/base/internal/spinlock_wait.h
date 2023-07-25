#ifndef CIC_BASE_INTERNAL_SPINLOCK_WAIT_H_
#define CIC_BASE_INTERNAL_SPINLOCK_WAIT_H_

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "cicflow/base/internal/scheduling_mode.h"

typedef struct {
  uint32_t from;
  uint32_t to;
  bool done;
} SPINLOCK_WAIT_TRANSITION;

uint32_t spinlock_wait(_Atomic(uint32_t) *w, int n, const SPINLOCK_WAIT_TRANSITION trans[],
                       SCHEDULING_MODE scheduling_mode);

void spinlock_wake(_Atomic(uint32_t) *w, bool all);

void spinlock_delay(_Atomic(uint32_t) *w, uint32_t value, int loop,
                    SCHEDULING_MODE scheduling_mode);

int spinlock_suggested_delay_ns(int loop);

#endif
