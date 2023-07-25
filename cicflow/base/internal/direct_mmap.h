#ifndef CIC_BASE_INTERNAL_ATOMIC_HOOK_H_
#define CIC_BASE_INTERNAL_ATOMIC_HOOK_H_

#include "cicflow/base/config.h"

#ifdef ABSL_HAVE_MMAP

#include <sys/mman.h>

#ifdef __linux__

#include <sys/types.h>
#ifdef __BIONIC__
#include <sys/syscall.h>
#else
#include <syscall.h>
#endif

#include <errno.h>
#include <linux/unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __mips__
#if defined(__BIONIC__) || !defined(__GLIBC__)
#include <asm/sgidefs.h>
#else
#include <sgidefs.h>
#endif  // __BIONIC__ || !__GLIBC__
#endif  // __mips__

#ifdef __BIONIC__
extern void *__mmap2(void *, size_t, int, int, int, size_t);
#if defined(__NR_mmap) && !defined(SYS_mmap)
#define SYS_mmap __NR_mmap
#endif
#ifndef SYS_munmap
#define SYS_munmap __NR_munmap
#endif
#endif  // __BIONIC__

#if defined(__NR_mmap2) && !defined(SYS_mmap2)
#define SYS_mmap2 __NR_mmap2
#endif

inline void *direct_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset) noexcept {
#if defined(__i386__) || defined(__ARM_ARCH_3__) || defined(__ARM_EABI__) || defined(__m68k__) || defined(__sh__) || \
    (defined(__hppa__) && !defined(__LP64__)) || (defined(__mips__) && _MIPS_SIM == _MIPS_SIM_ABI32) ||              \
    (defined(__PPC__) && !defined(__PPC64__)) || (defined(__riscv) && __riscv_xlen == 32) ||                         \
    (defined(__s390__) && !defined(__s390x__)) || (defined(__sparc__) && !defined(__arch64__))
  // On these architectures, implement mmap with mmap2.
  static int pagesize = 0;
  if (pagesize == 0) {
#if defined(__wasm__) || defined(__asmjs__)
    pagesize = getpagesize();
#else
    pagesize = sysconf(_SC_PAGESIZE);
#endif
  }
  if (offset < 0 || offset % pagesize != 0) {
    errno = EINVAL;
    return MAP_FAILED;
  }
#ifdef __BIONIC__
  return __mmap2(start, length, prot, flags, fd, size_t(offset / pagesize));
#else
  return (void *)(syscall(SYS_mmap2, start, length, prot, flags, fd, (unsigned long)(offset / pagesize));
#endif
#elif defined(__s390x__)
  unsigned long buf[6] = {unsigned long(start),    // NOLINT
                          unsigned long(length),   // NOLINT
                          unsigned long(prot),     // NOLINT
                          unsigned long(flags),    // NOLINT
                          unsigned long(fd),       // NOLINT
                          unsigned long(offset)};  // NOLINT
  return (void *)(syscall(SYS_mmap, buf));
#elif defined(__x86_64__)
#define MMAP_SYSCALL_ARG(x) ((uint64_t)(uintptr_t)(x))
  return (void *)(syscall(SYS_mmap, MMAP_SYSCALL_ARG(start), MMAP_SYSCALL_ARG(length), MMAP_SYSCALL_ARG(prot),
                          MMAP_SYSCALL_ARG(flags), MMAP_SYSCALL_ARG(fd), (uint64_t)(offset)));
#undef MMAP_SYSCALL_ARG
#else  // Remaining 64-bit aritectures.
  static_assert(sizeof(unsigned long) == 8, "Platform is not 64-bit");
  return (void *)(syscall(SYS_mmap, start, length, prot, flags, fd, offset));
#endif
}

inline int direct_munmap(void *start, size_t length) { return (int)(syscall(SYS_munmap, start, length)); }

#else  // !__linux__

inline void *direct_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset) {
  return mmap(start, length, prot, flags, fd, offset);
}

inline int direct_munmap(void *start, size_t length) { return munmap(start, length); }

#endif  // __linux__

#endif  // ABSL_HAVE_MMAP
#endif
