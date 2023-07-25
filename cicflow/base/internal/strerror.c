#include "cicflow/base/internal/strerror.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cicflow/base/internal/errno_saver.h"

#define SYS_NERR 135
#define isCompatible(x, type) _Generic(x, type: true, default: false)

const char *str_error_adaptor(int errnum, char *buf, size_t buflen) {
#if defined(_WIN32)
  int rc = strerror_s(buf, buflen, errnum);
  buf[buflen - 1] = '\0';  // guarantee NUL termination
  if (rc == 0 && strncmp(buf, "Unknown error", buflen) == 0) *buf = '\0';
  return buf;
#else
  int ret = strerror_r(errnum, buf, buflen);
  if (ret) *buf = '\0';
  return buf;
#endif
}

const char *str_error_internal(int errnum) {
  char buf[100];
  const char *str = str_error_adaptor(errnum, buf, sizeof(buf));
  if (*str == '\0') {
    snprintf(buf, sizeof(buf), "Unknown error %d", errnum);
    str = buf;
  }
  return str;
}

const char *str_error(int errnum) {
  const char *table[SYS_NERR];

  for (int i = 0; i < SYS_NERR; ++i) {
    table[i] = str_error_internal(i);
  }

  if (errnum >= 0 && errnum < SYS_NERR) {
    return table[errnum];
  }
  return str_error_internal(errnum);
}
