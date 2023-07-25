#ifndef CIC_BASE_INTERNAL_ERRNO_SAVER_H_
#define CIC_BASE_INTERNAL_ERRNO_SAVER_H_

#include <errno.h>

typedef struct {
  int saved_errno;
} ErrnoSaver;

#endif  // CIC_BASE_INTERNAL_ERRNO_SAVER_H_
