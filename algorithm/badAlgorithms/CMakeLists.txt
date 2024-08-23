cmake_minimum_required(VERSION 3.14)
project(BFSCleaingAfterMappingAlgorithm)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CXX g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror -pedantic")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Werror -pedantic")
endif()

set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)

get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
set(COMMON_ALGORITHM_DIR ${PARENT_DIR}/commonAlgorithm/)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_ALGORITHM_DIR}/include)
include_directories(${PARENT_DIR}/)
get_filename_component(GRANDPARENT_DIR ${PARENT_DIR} DIRECTORY)
include_directories(${GRANDPARENT_DIR}/common/)
include_directories(${GRANDPARENT_DIR})

add_library(
  empty SHARED
  ${PROJECT_SOURCE_DIR}/empty.cpp
)
add_library(
  incorrectlyRegisterAlgorithm SHARED
  ${PROJECT_SOURCE_DIR}/incorrectlyRegisterAlgorithm.cpp
)
add_library(
  throwsError SHARED
  ${PROJECT_SOURCE_DIR}/throwsError.cpp
)
set (BAD_LIB_DIR ${CMAKE_SOURCE_DIR}/../../badLib)
file(MAKE_DIRECTORY ${BAD_LIB_DIR})
set_target_properties(throwsError PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_LIB_DIR}
)

set_target_properties(incorrectlyRegisterAlgorithm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_LIB_DIR}
)
set_target_properties(empty PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_LIB_DIR}
)
set_target_properties(throwsError PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_LIB_DIR}
)

set_target_properties(incorrectlyRegisterAlgorithm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_LIB_DIR}
)
set_target_properties(empty PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_LIB_DIR}
)