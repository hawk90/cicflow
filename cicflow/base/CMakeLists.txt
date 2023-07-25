# ------------------------------------------------------------------------------
add_library(log_severity)
target_sources(log_severity PRIVATE log_severity.c log_severity.h)
target_link_libraries(log_severity PUBLIC config)
target_include_directories(log_severity PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")

# ------------------------------------------------------------------------------
add_library(raw_logging_internal)
target_sources(raw_logging_internal PRIVATE internal/raw_logging.c
                                            internal/raw_logging.h)
target_link_libraries(raw_logging_internal PUBLIC config)
target_include_directories(raw_logging_internal
                           PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")

# ------------------------------------------------------------------------------
# add_library(spinlock_wait)
# target_sources(
#   spinlock_wait PRIVATE internal/spinlock_wait.c internal/spinlock_linux.inc
#                         internal/spinlock_posix.inc internal/spinlock_wait.h)
# target_link_libraries(raw_logging_internal PUBLIC config)
# target_include_directories(spinlock_wait PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")

# ------------------------------------------------------------------------------
add_library(config INTERFACE)
target_include_directories(config INTERFACE "${CIC_COMMON_INCLUDE_DIRS}")

# ------------------------------------------------------------------------------
# add_library(dynamic_annotations)
# target_include_directories(dynamic_annotations INTERFACE
# "${CIC_COMMON_INCLUDE_DIRS}")

# ------------------------------------------------------------------------------
add_library(core_headers INTERFACE)
target_include_directories(core_headers INTERFACE "${CIC_COMMON_INCLUDE_DIRS}")

# ------------------------------------------------------------------------------
# add_library(malloc_internal)
# target_sources(
#   malloc_internal PRIVATE internal/low_level_alloc.c internal/low_level_alloc.h
#                           internal/direct_mmap.h)
# target_link_libraries(malloc_internal PUBLIC config)
# target_include_directories(malloc_internal PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")

# ------------------------------------------------------------------------------
# add_library(base_internal INTERFACE) target_include_directories(base_interanl
# INTERFACE "${CIC_COMMON_INCLUDE_DIRS}")

# ------------------------------------------------------------------------------
# add_library(base)
# target_sources(
#   base
#   PRIVATE internal/cycleclock.c
#           internal/spinlock.c
#           internal/sysinfo.c
#           internal/thread_identity.c
#           internal/unscaledcycleclock.c
#           call_once.h
#           internal/cycleclock.h
#           internal/cycleclock_config.h
#           internal/low_level_scheduling.h
#           internal/per_thread_tls.h
#           internal/spinlock.h
#           internal/sysinfo.h
#           internal/thread_identity.h
#           internal/tsan_mutex_interface.h
#           internal/unscaledcycleclock.h
#           internal/unscaledcycleclock_config.h)
# target_link_libraries(base PUBLIC config)
# target_include_directories(base PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")
