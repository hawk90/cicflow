# ------------------------------------------------------------------------------
add_executable(endian_test "")
target_sources(endian_test PRIVATE internal/endian_test.c)
target_link_libraries(endian_test PUBLIC config unity pthread)
target_include_directories(endian_test PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")
add_test(NAME endian_test COMMAND endian_test)

# ------------------------------------------------------------------------------
add_executable(config_test "")
target_sources(config_test PRIVATE config_test.c)
target_link_libraries(config_test PUBLIC config unity pthread)
target_include_directories(config_test PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")
add_test(NAME config_test COMMAND config_test)

# ------------------------------------------------------------------------------
add_executable(optimization_test "")
target_sources(optimization_test PRIVATE optimization_test.c)
target_link_libraries(optimization_test PUBLIC config unity)
target_include_directories(optimization_test
                           PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")
add_test(NAME optimization_test COMMAND optimization_test)

# ------------------------------------------------------------------------------
add_executable(raw_logging_test "")
target_sources(raw_logging_test PRIVATE raw_logging_test.c)
target_link_libraries(raw_logging_test PUBLIC config raw_logging_internal unity)
target_include_directories(raw_logging_test PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")
add_test(NAME raw_logging_test COMMAND raw_logging_test)

# ------------------------------------------------------------------------------
add_executable(prefetch_test "")
target_sources(prefetch_test PRIVATE prefetch_test.c)
target_link_libraries(prefetch_test PUBLIC config unity)
target_include_directories(prefetch_test PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")
add_test(NAME prefetch_test COMMAND prefetch_test)

# ------------------------------------------------------------------------------
add_executable(internal_prefetch_test "")
target_sources(internal_prefetch_test PRIVATE internal/prefetch_test.c)
target_link_libraries(internal_prefetch_test PUBLIC config unity)
target_include_directories(internal_prefetch_test
                           PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")
add_test(NAME internal_prefetch_test COMMAND internal_prefetch_test)

# ------------------------------------------------------------------------------
# add_executable(strerror_test "")
# target_sources(strerror_test PRIVATE internal/strerror_test.c)
# target_link_libraries(strerror_test PUBLIC config unity)
# target_include_directories(strerror_test PUBLIC "${CIC_COMMON_INCLUDE_DIRS}")
# add_test(NAME strerror_test COMMAND strerror_test)
