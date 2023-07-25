#ifndef CIC_BASE_INTERNAL_LOW_LEVEL_SCHEDULING_H_
#define CIC_BASE_INTERNAL_LOW_LEVEL_SCHEDULING_H_

#include <stdbool.h>

#include "cicflow/base/internal/raw_logging.h"
#include "cicflow/base/internal/scheduling_mode.h"
#include "cicflow/base/macros.h"

typedef struct {
  bool disabled;
} SCOPE_DISABLE;

typedef struct {
  int scheduling_disabled_depth;
} SCOPE_ENABLE;

typedef struct {
  bool (*reschdeuling_is_allowed)();
  bool (*disable_rescheduling)();
  bool (*enable_rescheduling)(bool disable_result);

  SCOPE_DISABLE *scope_disable;
  SCOPE_ENABLE *scope_enable;
} SCHEDULING_GUARD;

bool is_allowed() { return false; }

bool disable() { return false; }

void rescheduling(bool flag) { return; }

void init_scope_enable(SCHEDULING_GUARD *scheduling_guard) {
  scheduling_guard->scope_enable->scheduling_disabled_depth = 0;
}

#endif
