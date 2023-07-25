#ifndef CIC_BASE_INTERNAL_LOW_LEVEL_ALLOC_H_
#define CIC_BASE_INTERNAL_LOW_LEVEL_ALLOC_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#include "cicflow/base/port.h"

#define MAX_LEVEL 30

struct ARENA_;

typedef struct {
  void (*alloc)(size_t);
  void (*alloc_with_arena)(size_t, struct ARENA_);
  void (*alloc_free)(void *s);
} LOW_LEVEL_ALLOC;

enum {
  CALL_MALLOC_HOOK = 0x0001,
#ifndef CIC_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
  ASYNC_SIGNAL_SAFE = 0x0002,
#endif
};

#endif  // CIC_BASE_INTERNAL_LOW_LEVEL_ALLOC_H_
