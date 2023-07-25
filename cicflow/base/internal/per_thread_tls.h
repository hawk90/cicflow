#ifndef CIC_BASE_INTERNAL_PER_THREAD_TLS_H_
#define CIC_BASE_INTERNAL_PER_THREAD_TLS_H_

#include "cicflow/base/port.h"  // For CIC_HAVE_TLS

#if defined(CIC_PER_THREAD_TLS)
#error CIC_PER_THREAD_TLS cannot be directly set
#elif defined(CIC_PER_THREAD_TLS_KEYWORD)
#error CIC_PER_THREAD_TLS_KEYWORD cannot be directly set
#elif defined(CIC_HAVE_TLS)
#define CIC_PER_THREAD_TLS_KEYWORD __thread
#define CIC_PER_THREAD_TLS 1
#elif defined(_MSC_VER)
#define CIC_PER_THREAD_TLS_KEYWORD __declspec(thread)
#define CIC_PER_THREAD_TLS 1
#else
#define CIC_PER_THREAD_TLS_KEYWORD
#define CIC_PER_THREAD_TLS 0
#endif

#endif  // CIC_BASE_INTERNAL_PER_THREAD_TLS_H_
