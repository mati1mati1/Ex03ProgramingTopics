cmake_minimum_required(VERSION 3.14)
project(house_cleaner)

# if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
# endif()

set(CXX g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror -pedantic")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Werror -pedantic")
endif()

set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/MappingAlgorithm)
include_directories(${PROJECT_SOURCE_DIR}/test/include)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/34ad51b3dc4f922d8ab622491dd44fc2c39afee9.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

function(add_gtest_executable test_name)
  add_executable(${test_name} ${ARGN})
  target_link_libraries(${test_name} GTest::gtest_main)
  gtest_discover_tests(${test_name})
endfunction()

add_gtest_executable(
  CleaningRecordTest
  ${PROJECT_SOURCE_DIR}/src/Coordinate.cpp
  ${PROJECT_SOURCE_DIR}/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecordStep.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecord.cpp
  ${PROJECT_SOURCE_DIR}/test/CleaningRecordTest.cpp
)
add_gtest_executable(
  MeteredVacuumBatteryTest
  ${PROJECT_SOURCE_DIR}/src/MeteredVacuumBattery.cpp
  ${PROJECT_SOURCE_DIR}/test/MeteredVacuumBatteryTest.cpp
)
add_gtest_executable(
  HouseLocationTest
  ${PROJECT_SOURCE_DIR}/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/test/HouseLocationTest.cpp
)
add_gtest_executable(
  HouseTest
  ${PROJECT_SOURCE_DIR}/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumHouse.cpp
  ${PROJECT_SOURCE_DIR}/src/Coordinate.cpp
  ${PROJECT_SOURCE_DIR}/test/VacuumHouseTest.cpp
)
add_gtest_executable(
  VacuumParserTest
  ${PROJECT_SOURCE_DIR}/src/VacuumParser.cpp
  ${PROJECT_SOURCE_DIR}/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumHouse.cpp
  ${PROJECT_SOURCE_DIR}/src/MeteredVacuumBattery.cpp
  ${PROJECT_SOURCE_DIR}/src/Coordinate.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecordStep.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecord.cpp
  ${PROJECT_SOURCE_DIR}/test/VacuumParserTest.cpp
)
add_gtest_executable(
  MappingAlgorithmTest
  ${PROJECT_SOURCE_DIR}/src/VacuumParser.cpp
  ${PROJECT_SOURCE_DIR}/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumHouse.cpp
  ${PROJECT_SOURCE_DIR}/src/MeteredVacuumBattery.cpp
  ${PROJECT_SOURCE_DIR}/src/Coordinate.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumSimulator.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecordStep.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecord.cpp
  ${PROJECT_SOURCE_DIR}/src/OutFileWriter.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/MappingAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/SimultaneousMappingCleaningAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/MappingGraph.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/AbstractMappingAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/test/MappingAlgorithmTest.cpp
)
add_gtest_executable(
  SimultaneousMappingCleaningAlgorithmTest
  ${PROJECT_SOURCE_DIR}/src/VacuumParser.cpp
  ${PROJECT_SOURCE_DIR}/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumHouse.cpp
  ${PROJECT_SOURCE_DIR}/src/MeteredVacuumBattery.cpp
  ${PROJECT_SOURCE_DIR}/src/Coordinate.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumSimulator.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecordStep.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecord.cpp
  ${PROJECT_SOURCE_DIR}/src/OutFileWriter.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/SimultaneousMappingCleaningAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/MappingGraph.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/AbstractMappingAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/MappingAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/test/SimultaneousMappingCleaningAlgorithmTest.cpp
)
add_gtest_executable(
  OutFileWriterTest
  ${PROJECT_SOURCE_DIR}/src/VacuumParser.cpp
  ${PROJECT_SOURCE_DIR}/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/src/Coordinate.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumHouse.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecord.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecordStep.cpp
  ${PROJECT_SOURCE_DIR}/src/OutFileWriter.cpp
  ${PROJECT_SOURCE_DIR}/test/OutFileWriterTest.cpp
)

add_executable(
  myrobot
  ${PROJECT_SOURCE_DIR}/src/CleaningRecord.cpp
  ${PROJECT_SOURCE_DIR}/src/CleaningRecordStep.cpp
  ${PROJECT_SOURCE_DIR}/src/Coordinate.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumHouse.cpp
  ${PROJECT_SOURCE_DIR}/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/src/OutFileWriter.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumSimulator.cpp
  ${PROJECT_SOURCE_DIR}/src/VacuumParser.cpp
  ${PROJECT_SOURCE_DIR}/src/MeteredVacuumBattery.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/MappingAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/MappingGraph.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/SimultaneousMappingCleaningAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/src/MappingAlgorithm/AbstractMappingAlgorithm.cpp
  ${PROJECT_SOURCE_DIR}/main.cpp
)

