cmake_minimum_required(VERSION 3.5)

# Compiler id for Apple Clang is now AppleClang.
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif(POLICY CMP0025)

# if command can use IN_LIST
if(POLICY CMP0057)
  cmake_policy(SET CMP0057 NEW)
endif(POLICY CMP0057)

# Project version variables are the empty string if version is unspecified
if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif(POLICY CMP0048)

# option() honor variables
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif(POLICY CMP0077)

# Allow the user to specify the MSVC runtime
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif(POLICY CMP0091)

########################################################################################################################
project(
  cicflow
  VERSION 0.1.0
  LANGUAGES C)

########################################################################################################################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

########################################################################################################################
# /bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)

# include current path
list(APPEND CIC_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

########################################################################################################################
add_subdirectory(cicflow)
