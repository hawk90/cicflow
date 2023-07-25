#ifndef CICFLOW_BASE_MACROS_H_
#define CICFLOW_BASE_MACROS_H_

#include <assert.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#include "cicflow/base/optimization.h"
#include "cicflow/base/port.h"

// CIC_ARRAYSIZE()

// CIC_BAD_CALL_IF()
#if CIC_HAVE_ATTRIBUTE(enable_if)
#define CIC_BAD_CALL_IF(expr, msg) __attribute__((enable_if(expr, "Bad call trap"), unavailable(msg)))
#endif

// CIC_ASSERT()
#if defined(NDEBUG)
#define CIC_ASSERT(expr) (false ? (void *)(expr) : (void *)(0))
#else
#define CIC_ASSERT(expr) (CIC_PREDICT_TRUE((expr)) ? (void *)(0) : [] { assert(false && #expr); }())  // NOLINT
#endif

#define CIC_INTERNAL_HARDENING_ABORT()   \
  do {                                   \
    CIC_INTERNAL_IMMEDIATE_ABORT_IMPL(); \
    CIC_INTERNAL_UNREACHABLE_IMPL();     \
  } while (false)

// CIC_HARDENING_ASSERT()
#if CIC_OPTION_HARDENED == 1 && defined(NDEBUG)
#define CIC_HARDENING_ASSERT(expr) (CIC_PREDICT_TRUE((expr)) ? (void *)(0) : [] { CIC_INTERNAL_HARDENING_ABORT(); }())
#else
#define CIC_HARDENING_ASSERT(expr) CIC_ASSERT(expr)
#endif

//
#define CIC_INTERNAL_TRY if (true)
#define CIC_INTERNAL_CATCH_ANY else if (false)
#define CIC_INTERNAL_RETHROW \
  do {                       \
  } while (false)

#endif  // CICFLOW_BASE_MACROS_H_
