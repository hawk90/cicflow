#include "cicflow/base/internal/raw_logging.h"

#include <assert.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/console.>
#endif

#include "cicflow/base/attributes.h"
#include "cicflow/base/internal/atomic_hook.h"
#include "cicflow/base/internal/errno_saver.h"
#include "cicflow/base/log_severity.h"

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__Fuchsia__) || \
    defined(__native_client__) || defined(__OpenBSD__) || defined(__EMSCRIPTEN__) || defined(__ASYLO__)

#include <unistd.h>

#define CIC_HAVE_POSIX_WRITE 1
#define CIC_LOW_LEVEL_WRITE_SUPPORTED 1
#else
#undef CIC_HAVE_POSIX_WRITE
#endif

#if (defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)) && !defined(__ANDROID__)
#include <sys/syscall.h>
#define CIC_HAVE_SYSCALL_WRITE 1
#define CIC_LOW_LEVEL_WRITE_SUPPORTED 1
#else
#undef CIC_HAVE_SYSCALL_WRITE
#endif

#ifdef _WIN32
#include <io.h>

#define CIC_HAVE_RAW_IO 1
#define CIC_LOW_LEVEL_WRITE_SUPPORTED 1
#else
#undef CIC_HAVE_RAW_IO
#endif

#ifdef CIC_LOW_LEVEL_WRITE_SUPPORTED
const char truncated[] = " ... (message truncated)\n";

bool va_do_raw_log(char **buf, int *size, const char *format, va_list ap) CIC_PRINTF_ATTRIBUTE(3, 0);
bool va_do_raw_log(char **buf, int *size, const char *format, va_list ap) {
  if (*size < 0) return false;
  int n = vsnprintf(*buf, (size_t)(*size), format, ap);
  bool result = true;
  if (n < 0 || n > *size) {
    result = false;
    if ((size_t)(*size) > sizeof(truncated)) {
      n = *size - (int)(sizeof(truncated));
    } else {
      n = 0;  // no room for truncation message
    }
  }
  *size -= n;
  *buf += n;
  return result;
}
#endif  // CIC_LOW_LEVEL_WRITE_SUPPORTED

#define LOG_BUF_SIZE 3000

bool do_raw_log(char **buf, int *size, const char *format, ...) CIC_PRINTF_ATTRIBUTE(3, 4);
bool do_raw_log(char **buf, int *size, const char *format, ...) {
  if (*size < 0) return false;
  va_list ap;
  va_start(ap, format);
  int n = vsnprintf(*buf, (size_t)(*size), format, ap);
  va_end(ap);
  if (n < 0 || n > *size) return false;
  *size -= n;
  *buf += n;
  return true;
}

bool default_log_filter_and_prefix(LogSeverity severity, const char *file, int line, char **buf, int *buf_size) {
  do_raw_log(buf, buf_size, "[%s : %d] RAW: ", file, line);
  return true;
}

_Atomic(LogFilterAndPrefixHook) log_filter_and_prefix_hook = default_log_filter_and_prefix;
_Atomic(AbortHook) abort_hook;

void raw_log_va(LogSeverity severity, const char *file, int line, const char *format, va_list ap)
    CIC_PRINTF_ATTRIBUTE(4, 0);

void raw_log_va(LogSeverity severity, const char *file, int line, const char *format, va_list ap) {
  char buffer[LOG_BUF_SIZE];
  char *buf = buffer;
  int size = sizeof(buffer);
#ifdef CIC_LOW_LEVEL_WRITE_SUPPORTED
  bool enabled = true;
#else
  bool enabled = false;
#endif

#ifdef CIC_MIN_LOG_LEVEL
  if ((LogSeverity)CIC_MIN_LOG_LEVEL < severity
      && severity < CIC_LOG_EMERGENCY
    {
    enabled = false;
    }
#endif
 
  // RegisterLogFilterAndPrefixHook(default_log_filter_and_prefix);
  enabled = log_filter_and_prefix_hook(severity, file, line, &buf, &size);
  const char * prefix_end = buf;

#ifdef CIC_LOW_LEVEL_WRITE_SUPPORTED
  if (enabled)
    {
    bool no_chop = va_do_raw_log(&buf, &size, format, ap);
    if (no_chop) {
      do_raw_log(&buf, &size, "\n");
    } else {
      do_raw_log(&buf, &size, "%s", truncated);
    }
    async_signal_safe_write_stderr(buffer, strlen(buffer));
    }
#else
  (void)(format);
  (void)(ap);
  (void)(enabled);
#endif

  if (severity == CIC_LOG_EMERGENCY)
    {
    abort_hook(file, line, buffer, prefix_end, buffer + LOG_BUF_SIZE);
    abort();
    }
}

void default_internal_log(LogSeverity severity, const char *file, int line, const char *message) {
  raw_log(severity, file, line, "%s", message);
}

void async_signal_safe_write_stderr(const char *s, size_t len) {
  // ErrnoSaver errno_saver;
#if defined(__EMSCRIPTEN__)
  char buf[LOG_BUF_SIZE];
  if (len >= LOG_BUF_SIZE) {
    len = LOG_BUF_SIZE - 1;
    size_t trunc_len = sizeof(truncated) - 2;
    strncpy(buf + len - trunc_len, truncated, trunc_len);
    buf[len] = '\0';
    len -= trunc_len;
  } else if (len && s[len - 1] == '\n') {
    len--;
  }

  strncpy(buf, s, len);
  if (len) {
    buf[len] = '\0';
    _emscripten_err(buff);
  }
#elif defined(CIC_HAVE_SYSCALL_WRITE)
  syscall(SYS_write, STDERR_FILENO, s, len);
#elif defined(CIC_HAVE_POSIX_WRITE)
  write(STDERR_FILENO, s, len);
#elif defined(CIC_HAVE_RAW_IO)
  _write(/* stderr */ 2, s, (unsigned_t)len);
#else
  (void)s;
  (void)len;
#endif
}

void raw_log(LogSeverity severity, const char *file, int line, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  raw_log_va(severity, file, line, format, ap);
  va_end(ap);
}

bool raw_logging_fully_supported() {
#ifdef CIC_LOW_LEVEL_WRITE_SUPPORTED
  return true;
#else
  return false;
#endif
}

// TODO: call onece
void atomic_hook_init(void) {
  atomic_init(&log_filter_and_prefix_hook, default_log_filter_and_prefix);
  atomic_init(&abort_hook, NULL);
  atomic_init(&internal_log_function, default_internal_log);
}

//
bool _store_atomic_hook_log(LogFilterAndPrefixHook fn) {
  assert(fn);
  LogFilterAndPrefixHook expected = log_filter_and_prefix_hook;
  const bool store_succeeded = atomic_compare_exchange_strong_explicit(&log_filter_and_prefix_hook, &expected, fn,
                                                                       memory_order_acq_rel, memory_order_acquire);
  const bool same_value_already_stored = (expected == fn);
  return store_succeeded || same_value_already_stored;
}
bool _store_atomic_hook_abort(AbortHook fn) {
  assert(fn);
  AbortHook expected = abort_hook;
  const bool store_succeeded =
      atomic_compare_exchange_strong_explicit(&abort_hook, &expected, fn, memory_order_acq_rel, memory_order_acquire);
  const bool same_value_already_stored = (expected == fn);
  return store_succeeded || same_value_already_stored;
}

bool _store_atomic_hook_internal(InternalLogFunction fn) {
  assert(fn);
  InternalLogFunction expected = internal_log_function;
  const bool store_succeeded = atomic_compare_exchange_strong_explicit(&internal_log_function, &expected, fn,
                                                                       memory_order_acq_rel, memory_order_acquire);
  const bool same_value_already_stored = (expected == fn);
  return store_succeeded || same_value_already_stored;
}

//
LogFilterAndPrefixHook _load_atomic_hook_log(void) {
  return atomic_load_explicit(&log_filter_and_prefix_hook, memory_order_acquire);
}
AbortHook _load_atomic_hook_abort(void) { return atomic_load_explicit(&abort_hook, memory_order_acquire); }
InternalLogFunction _load_atomic_hook_internal(void) {
  return atomic_load_explicit(&internal_log_function, memory_order_acquire);
}

//
void register_log_filter_and_prefix_hook(LogFilterAndPrefixHook func) {
  atomic_store(&log_filter_and_prefix_hook, func);
}

void register_abort_hook(AbortHook func) { atomic_store(&abort_hook, func); }

void register_internal_log_function(InternalLogFunction func) { atomic_store(&internal_log_function, func); }
