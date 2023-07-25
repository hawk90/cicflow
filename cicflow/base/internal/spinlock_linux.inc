#include <limits.h>
#include <linux/futex.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#include "cicflow/base/attributes.h"
#include "cicflow/base/internal/errno_saver.h"

static_assert(sizeof(_Atomic(uint32_t)) == sizeof(int),
              "SpinLock lockword has the wrong size for a futex");

#ifdef __BIONIC__
#ifndef SYS_futex
#define SYS_futex __NR_futex
#endif
#ifndef FUTEX_PRIVATE_FLAG
#define FUTEX_PRIVATE_FLAG 128
#endif
#endif

#if defined(__NR_futex_time64) && !defined(SYS_futex_time64)
#define SYS_futex_time64 __NR_futex_time64
#endif

#if defined(SYS_futex_time64) && !defined(SYS_futex)
#define SYS_futex SYS_futex_time64
#endif

void cic_linternal_spinlock_delay(_Atomic(uint32_t) *w, uint32_t value, int, SCHEDULING_MODE) {
  ErrnoSaver errno_saver;
  syscall(SYS_futex, w, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, value, NULL);
}

void cic_internal_spinlock_wake(_Atomic(uint32_t) *w, bool all) {
  syscall(SYS_futex, w, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, all ? INT_MAX : 1, 0);
}
