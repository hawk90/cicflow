#ifndef CICFLOW_BASE_PREFETCH_H_
#define CICFLOW_BASE_PREFETCH_H_

#include "cicflow/base/config.h"

#if defined(CICFLOW_INTERNAL_HAVE_SSE)
#include <xmmintrin.h>
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1900 && (defined(_M_X64) || defined(_M_IX86))
#include <intrin.h>
#pragma intrinsic(_mm_prefetch)
#endif

#if CIC_HAVE_BUILTIN(__builtin_prefetch) || defined(__GNUC__)

#define CICFLOW_HAVE_PREFETCH 1

void prefetch_to_local_cache(const void *addr) { __builtin_prefetch(addr, 0, 3); }

void prefetch_to_local_cache_nta(const void *addr) { __builtin_prefetch(addr, 0, 0); }

void prefetch_to_local_cache_for_write(const void *addr) {
#if defined(__x86_64__)
  asm("prefetchw (%0)" : : "r"(addr));
#else
  __builtin_prefetch(addr, 1, 3);
#endif
}

#elif defined(CICFLOW_INTERNAL_HAVE_SSE)

#define CICFLOW_HAVE_PREFETCH 1

void prefetch_to_local_cache(const void *addr) { _mm_prefetch((const char *)(addr), _MM_HINT_T0); }

void prefetch_to_local_cache_nta(const void *addr) { _mm_prefetch((const char *)(addr), _MM_HINT_NTA); }

void prefetch_to_local_cache_for_write(const void *addr) {
#if defined(_MM_HINT_ET0)
  _mm_prefetch((const char *)(addr), _MM_HINT_ET0);
#elif !defined(_MSC_VER) && defined(__x86_64__)
  asm("prefetchw (%0)" : : "r"(addr));
#endif
}

#else

void prefetch_to_local_cache(const void *addr) {}
void prefetch_to_local_cache_nta(const void *addr) {}
void prefetch_to_local_cache_for_write(const void *addr) {}

#endif

#endif  // CICFLOW_BASE_PREFETCH_H_
