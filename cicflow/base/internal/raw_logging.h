#ifndef CIC_BASE_INTERNAL_RAW_LOGGING_H_
#define CIC_BASE_INTERNAL_RAW_LOGGING_H_

#include <libgen.h>
#include <stdbool.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/config.h"
#include "cicflow/base/internal/atomic_hook.h"
#include "cicflow/base/log_severity.h"
#include "cicflow/base/macros.h"
#include "cicflow/base/optimization.h"
#include "cicflow/base/port.h"

#define CIC_RAW_LOG_INTERNAL_MAYBE_UNREACHABLE_CIC_LOG_INFO
#define CIC_RAW_LOG_INTERNAL_MAYBE_UNREACHABLE_CIC_LOG_WARNING
#define CIC_RAW_LOG_INTERNAL_MAYBE_UNREACHABLE_CIC_LOG_ERROR
#define CIC_RAW_LOG_INTERNAL_MAYBE_UNREACHABLE_CIC_LOG_FATAL CIC_UNREACHABLE()
#define CIC_RAW_LOG_INTERNAL_MAYBE_UNREACHABLE_CIC_LOG_LEVEL(severity)

typedef bool (*LogFilterAndPrefixHook)(LogSeverity severity, const char *file, int line, char **buf,
                                       int *buf_size);
typedef void (*AbortHook)(const char *file, int line, const char *buf_start, const char *prefix_end,
                          const char *buf_end);
typedef void (*InternalLogFunction)(LogSeverity severity, const char *file, int line,
                                    const char *message);

_Atomic(InternalLogFunction) internal_log_function;

void raw_log(LogSeverity severity, const char *file, int line, const char *format, ...);

#define CIC_RAW_LOG(severity, ...)                                           \
  do {                                                                       \
    const char *cic_raw_log_internal_basename = basename(__FILE__);          \
    raw_log(severity, cic_raw_log_internal_basename, __LINE__, __VA_ARGS__); \
    CIC_RAW_LOG_INTERNAL_MAYBE_UNREACHABLE_##severity;                       \
  } while (0)

#define CIC_RAW_CHECK(condition, message)                                     \
  do {                                                                        \
    if (CIC_PREDICT_FALSE(!(condition))) {                                    \
      CIC_RAW_LOG(CIC_LOG_FATAL, "Check %s failed: %s", #condition, message); \
    }                                                                         \
  } while (0)

#define CIC_INTERNAL_LOG(severity, message)                                            \
  do {                                                                                 \
    const char *cic_raw_log_internal_filename = __FILE__;                              \
    internal_log_function(severity, cic_raw_log_internal_filename, __LINE__, message); \
  } while (0)

void raw_log(LogSeverity severity, const char *file, int line, const char *format, ...)
    CIC_PRINTF_ATTRIBUTE(4, 5);

#define CIC_INTERNAL_CHECK(condition, message)                                \
  do {                                                                        \
    if (CIC_PREDICT_FALSE(!(condition))) {                                    \
      CIC_RAW_LOG(CIC_LOG_FATAL, "Check %s failed: %s", #condition, message); \
    }                                                                         \
  } while (0)

#ifndef NDEBUG

#define CIC_RAW_DLOG(severity, ...) CIC_RAW_LOG(severity, __VA_ARGS__)
#define CIC_RAW_DCHECK(condition, message) CIC_RAW_CHECK(condition, message)

#else  // NDEBUG

#define CIC_RAW_DLOG(severity, ...) \
  while (false) CIC_RAW_LOG(severity, __VA_ARGS__)
#define CIC_RAW_DCHECK(condition, message) \
  while (false) CIC_RAW_CHECK(condition, message)

#endif  // NDEBUG

void async_signal_safe_write_stderr(const char *s, size_t len);

// TODO: basename does not work MSVC
// const char *Basename(const char *fname, int offset) {
//   return offset == 0 || fname[offset - 1] == '/' || fname[offset - 1] == '\\'
//              ? fname + offset
//              : Basename(fname, offset - 1);
// }

bool raw_logging_fully_supported();

void atomic_hook_init(void);

LogFilterAndPrefixHook _load_atomic_hook_log(void);
AbortHook _load_atomic_hook_abort();
InternalLogFunction _load_atomic_hook_internal();

bool _store_atomic_hook_log(LogFilterAndPrefixHook fn);
bool _store_atomic_hook_abort(AbortHook fn);
bool _store_atomic_hook_internal(InternalLogFunction fn);

#define atomic_hook_load(func)                       \
  _Generic((func),                                   \
      LogFilterAndPrefixHook: _load_atomic_hook_log, \
      AbortHook: _load_atomic_hook_abort,            \
      InternalLogFunction: _load_atomic_hook_internal)(void)

#define atomic_hook_store(func)                       \
  _Generic((func),                                    \
      LogFilterAndPrefixHook: _store_atomic_hook_log, \
      AbortHook: _store_atomic_hook_abort,            \
      InternalLogFunction: _store_atomic_hook_internal)(func)

void register_log_filter_and_prefix_hook(LogFilterAndPrefixHook func);
void register_abort_hook(AbortHook func);
void register_internal_log_function(InternalLogFunction func);

#endif  // CIC_BASE_INTERNAL_RAW_LOGGING_H_
