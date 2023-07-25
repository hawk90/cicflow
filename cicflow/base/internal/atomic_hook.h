#ifndef CIC_BASE_INTERNAL_ATOMIC_HOOK_H_
#define CIC_BASE_INTERNAL_ATOMIC_HOOK_H_

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#include "cicflow/base/log_severity.h"

#if defined(_MSC_VER)
#define CIC_HAVE_WORKING_ATOMIC_POINTER 0
#else
#define CIC_HAVE_WORKING_ATOMIC_POINTER 1
#endif

typedef void (*FuncPtr)(void *);

// typedef struct
// {
//   _Atomic FuncPtr current_func;
//   FuncPtr default_func;
// } AtomicHook;

#endif  // CIC_BASE_INTERNAL_ATOMIC_HOOK_H_
