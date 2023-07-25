include(CMakeParseArguments)

# cic_c_library()
#
# CMake function to imitate Bazel's cc_library rule.
function(cic_c_library)
  cmake_parse_arguments(CIC_C_LIB "PUBLIC" "NAME"
                        "HDRS;SRCS;COPTS;DEFINES;LINKOPTS;DEPS" ${ARGN})

  # FIXIT:
  set(_NAME "${CIC_LIB_NAME}")

  # Check if this is a header-only library
  set(CIC_C_SRCS "${CIC_C_LIB_SRCS}")
  foreach(src_file IN LISTS CIC_C_SRCS)
    if(${src_file} MATCHES ".*\\.(h|inc)")
      list(REMOVE_ITEM CIC_C_SRCS "${src_file}")
    endif()
  endforeach()

  if(CIC_C_SRCS STREQUAL "")
    set(CIC_C_LIB_IS_INTERFACE 1)
  else()
    set(CIC_C_LIB_IS_INTERFACE 0)
  endif()

  #
  if(NOT CIC_C_LIB_ISINTERFACE)
    add_library(${_NAME} "")
    target_sources(${_NAME} PRIVATE ${CIC_C_LIB_SRCS} ${CIC_C_LIB_HDRS})
    target_link_libraries(
      ${_NAME}
      PUBLIC ${CIC_C_LIB_DEPS}
      PRIVATE ${CIC_C_LIB_LINKOPTS} ${CIC_DEFAULT_LINKOPTS})

    target_include_directories(${_NAME} PUGLIC ${CIC_COMMON_INCLUDE_DIRS})
    # target_compile_options() target_compile_definitions()
  else()
    add_library(${_NAME} INTERFACE)
    target_include_directories(${_NAME} INTERFACE ${CIC_COMMON_INCLUDE_DIRS})
    # target_compile_definitions()
  endif()
endfunction()
