# To-Do List
## Task 1. abseil
1. base
  - [x] policy_checks.h
  - [x] attributes.h  
  - [x] options.h  
  - [x] config.h  
  - [ ] config_test.h
  - [x] optimization.h  
  - [ ] optimization_test.h
  - [x] port.h  
  - [x] macros.h  
  - [x] log_severity.h
  - [x] log_severity.c
  - [x] log_severity_test.c
  - [x] internal/errno_saver.h
  - [ ] internal/atomic_hook.h
  - [ ] internal/unaligned_access.h
  - [ ] internal/endian.h
  - [ ] internal/raw_logging.h
  - [ ] internal/raw_logging.c

  - [ ] dynamic_annotations.h
  - [ ] thread_annotations.h
  - [ ] call_once.h
--
  - [ ] internal/cycleclock.h
  - [ ] internal/cycleclock.c
  - [ ] internal/cycleclock_config.h
  - [ ] internal/direct_mmap.h
  - [ ] internal/dynamic_annotations.h
  - [ ] internal/fast_type_id.h
  - [ ] internal/hide_ptr.h
  - [ ] internal/identity.h
  - [ ] internal/inline_variable.h
  - [ ] internal/invoke.h
  - [ ] internal/low_level_alloc.h
  - [ ] internal/low_level_alloc.c
  - [ ] internal/low_level_scheduling.h
  - [ ] internal/per_thread_tls.h
  - [ ] internal/prefetch.h
  - [ ] internal/scheduling_mode.h
  - [ ] internal/scoped_set_env.h
  - [ ] internal/scoped_set_env.c
  - [ ] internal/spinlock.h
  - [ ] internal/spinlock.c
  - [ ] internal/spinlock_linux.inc
  - [ ] internal/spinlock_posix.inc
  - [ ] internal/spinlock_wait.h
  - [ ] internal/spinlock_wait.c
  - [ ] internal/spinlock_benchmark.c
  - [ ] internal/strerror.h
  - [ ] internal/strerror.c
  - [ ] internal/strerror_benchmark.c
  - [ ] internal/sysinfo.h
  - [ ] internal/sysinfo.c
  - [ ] internal/thread_annotations.h
  - [ ] internal/thread_identity.h
  - [ ] internal/thread_identity.c
  - [ ] internal/thread_identity_benchmark.c
  - [ ] internal/throw_delegate.h
  - [ ] internal/throw_delegate.c
  - [ ] internal/tsan_mutex_interface.h
  - [ ] internal/unscaledcycleclock.h
  - [ ] internal/unscaledcycleclock.c
  - [ ] internal/unscaledcycleclock_config.h
  - [ ] internal/pretty_function.h  // does not support in C.


2. debugging

3. log

4. profiling

5. container

6. hash

## Task 2. packet

- - - -
# Unit Test Frameworks
check, cmoka, unity

- - - -
# Project Convention
> GNU or Linux Kernel  

```bash
mkdir build
cd build
cmake ..  # configure the project
cmake --build . --target main  # build the executable
./main some commandline args
```
