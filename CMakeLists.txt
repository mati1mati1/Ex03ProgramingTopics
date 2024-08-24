cmake_minimum_required(VERSION 3.14)
project(CleaningVacuom)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/some_output
    COMMAND ${CMAKE_COMMAND} -E env ROOT_DIR=${CMAKE_SOURCE_DIR} ${CMAKE_COMMAND} -E echo "ROOT_DIR is ${ROOT_DIR}"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Setting ROOT_DIR environment variable for custom command"
  )
  add_subdirectory(algorithm/badAlgorithms)
  enable_testing()
endif()

add_subdirectory(simulator)
add_subdirectory(algorithm/Algo_323012971_315441972_Orignal)
add_subdirectory(algorithm/Algo_323012971_315441972_Simultaneous)
