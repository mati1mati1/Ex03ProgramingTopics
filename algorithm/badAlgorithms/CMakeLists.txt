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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
  faultyAlgorithm SHARED
  ${PROJECT_SOURCE_DIR}/faultyAlgorithm.cpp
)
add_library(
  timingOut SHARED
  ${PROJECT_SOURCE_DIR}/timingOut.cpp
)
add_library(
  timingOutSometimes SHARED
  ${PROJECT_SOURCE_DIR}/timingOutSometimes.cpp
)
add_library(
  timingOutSometimesFaultySometimes SHARED
  ${PROJECT_SOURCE_DIR}/timingOutSometimesFaultySometimes.cpp
)

set(BAD_LIB_DIR ${CMAKE_SOURCE_DIR}/badLib)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(BAD_TIMEOUT_DIR ${CMAKE_SOURCE_DIR}/runtimeBadLib)
set(BAD_SOMETIMETIMEOUT_DIR ${CMAKE_SOURCE_DIR}/sometimetimeout)
set(SEMI_FAULTY_DIR ${CMAKE_SOURCE_DIR}/semifaultyAlgorithms)
set(BAD_TIMEOUTFAULTY_DIR ${CMAKE_SOURCE_DIR}/timeoutfaultyAlgorithms)
set(ALL_LIB_DIR ${CMAKE_SOURCE_DIR}/allLib)

file(MAKE_DIRECTORY ${LIB_DIR})
file(MAKE_DIRECTORY ${BAD_LIB_DIR})
file(MAKE_DIRECTORY ${BAD_TIMEOUT_DIR})
file(MAKE_DIRECTORY ${BAD_SOMETIMETIMEOUT_DIR})
file(MAKE_DIRECTORY ${SEMI_FAULTY_DIR})
file(MAKE_DIRECTORY ${BAD_TIMEOUTFAULTY_DIR})
file(MAKE_DIRECTORY ${ALL_LIB_DIR})

set_target_properties(empty PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR}
)
set_target_properties(faultyAlgorithm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${SEMI_FAULTY_DIR}
)
set_target_properties(timingOut PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_TIMEOUT_DIR}
)
set_target_properties(timingOutSometimes PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_SOMETIMETIMEOUT_DIR}
)
set_target_properties(timingOutSometimesFaultySometimes PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BAD_TIMEOUTFAULTY_DIR}
)
add_custom_command(TARGET empty POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:empty>
    ${BAD_LIB_DIR}/
)
add_custom_command(TARGET empty POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:empty>
    ${ALL_LIB_DIR}/
)
add_custom_command(TARGET faultyAlgorithm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:faultyAlgorithm>
    ${ALL_LIB_DIR}/
)
add_custom_command(TARGET timingOut POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:timingOut>
    ${ALL_LIB_DIR}/
)
add_custom_command(TARGET timingOutSometimes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:timingOutSometimes>
    ${ALL_LIB_DIR}/
)
add_custom_command(TARGET timingOutSometimesFaultySometimes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:timingOutSometimesFaultySometimes>
    ${ALL_LIB_DIR}/
)