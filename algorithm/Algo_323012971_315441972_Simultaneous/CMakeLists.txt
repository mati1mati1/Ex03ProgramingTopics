cmake_minimum_required(VERSION 3.14)
project(BFSSimultaneousMappingAndCleaningAlgorithm)

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
  Algo_323012971_315441972_Simultaneous SHARED
  ${PROJECT_SOURCE_DIR}/src/Algo_323012971_315441972_Simultaneous.cpp
  ${COMMON_ALGORITHM_DIR}/src/MappingAlgorithm.cpp
  ${COMMON_ALGORITHM_DIR}/src/MappingGraph.cpp
  ${GRANDPARENT_DIR}/common/src/Coordinate.cpp
  ${GRANDPARENT_DIR}/common/src/HouseLocation.cpp
)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set_target_properties(Algo_323012971_315441972_Simultaneous PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

  file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/badAndGoodLib)
  file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/allLib)
  add_custom_command(TARGET Algo_323012971_315441972_Simultaneous POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      $<TARGET_FILE:Algo_323012971_315441972_Simultaneous>
      ${CMAKE_SOURCE_DIR}/badAndGoodLib/
  )
  add_custom_command(TARGET Algo_323012971_315441972_Simultaneous POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      $<TARGET_FILE:Algo_323012971_315441972_Simultaneous>
      ${CMAKE_SOURCE_DIR}/allLib/
  )
endif()