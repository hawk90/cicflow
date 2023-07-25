#ifndef CIC_BASE_INTERNAL_PREFETCH_H_
#define CIC_BASE_INTERNAL_PREFETCH_H_

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#include "cicflow/base/prefetch.h"

#ifdef __SSE__
#include <xmmintrin.h>
#endif

#if defined(_MSC_VER) && defined(CIC_INTERNAL_HAVE_SSE)
#include <intrin.h>
#pragma intrinsic(_mm_prefetch)
#endif

void prefetch_t0(const void* address) { prefetch_to_local_cache(address); }

void prefetch_nta(const void* address) { prefetch_to_local_cache_nta(address); }

#if CIC_HAVE_BUILTIN(__builtin_prefetch) || defined(__GNUC__)

#define CIC_INTERNAL_HAVE_PREFETCH 1

void prefetch_t1(const void* addr) { __builtin_prefetch(addr, 0, 2); }
void prefetch_t2(const void* addr) { __builtin_prefetch(addr, 0, 1); }

#elif defined(CIC_INTERNAL_HAVE_SSE)

#define CIC_INTERNAL_HAVE_PREFETCH 1

void prefetch_t1(const void* addr) { _mm_prefetch((const char*)(addr), _MM_HINT_T1); }
void prefetch_t2(const void* addr) { _mm_prefetch((const char*)(addr), _MM_HINT_T2); }

#else
void prefetch_t1(const void*) {}
void prefetch_t2(const void*) {}
#endif

#endif
