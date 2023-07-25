configure_file(${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt.in
               ${CMAKE_BINARY_DIR}/unity-external/CMakeLists.txt)

execute_process(
  COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unity-external)
if(result)
  message(FATAL_ERROR "CMake step for unity failed: ${result}")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unity-external)
if(result)
  message(FATAL_ERROR "Build step for unity failed: ${result}")
endif()

add_subdirectory(${unity_src_dir} ${unity_build_dir} EXCLUDE_FROM_ALL)
