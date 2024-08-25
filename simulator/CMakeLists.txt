cmake_minimum_required(VERSION 3.14)
project(simulator)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CXX g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(${PROJECT_SOURCE_DIR}/algorithm/)
include_directories(${PROJECT_SOURCE_DIR}/include/)
get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
include_directories(${PARENT_DIR})
include_directories(${PARENT_DIR}/common)

set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.zip 
    USES_TERMINAL_DOWNLOAD TRUE 
    GIT_PROGRESS TRUE   
    DOWNLOAD_NO_EXTRACT FALSE
)
FetchContent_MakeAvailable(Boost)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include_directories(${PROJECT_SOURCE_DIR}/test/include/)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror -pedantic -rdynamic ")
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/34ad51b3dc4f922d8ab622491dd44fc2c39afee9.zip
  )
  if(NOT DEFINED ROOT_DIR)
    set(ROOT_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "Root directory of the project")
  endif()
  FetchContent_MakeAvailable(googletest)
  enable_testing()
  include(GoogleTest)

  function(add_gtest_executable test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name} GTest::gtest_main)
    gtest_discover_tests(${test_name})
    target_compile_definitions(${test_name} PRIVATE ROOT_DIR="${ROOT_DIR}")
  endfunction()

  add_gtest_executable(
    CleaningRecordTest
    ${PARENT_DIR}/common/src/Coordinate.cpp
    ${PARENT_DIR}/common/src/HouseLocation.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecordStep.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecord.cpp
    ${PROJECT_SOURCE_DIR}/test/CleaningRecordTest.cpp
  )
  add_gtest_executable(
    MeteredVacuumBatteryTest
    ${PROJECT_SOURCE_DIR}/MeteredVacuumBattery.cpp
    ${PROJECT_SOURCE_DIR}/test/MeteredVacuumBatteryTest.cpp
  )
  add_gtest_executable(
    HouseLocationTest
    ${PARENT_DIR}/common/src/HouseLocation.cpp
    ${PROJECT_SOURCE_DIR}/test/HouseLocationTest.cpp
  )
  add_gtest_executable(
    HouseTest
    ${PARENT_DIR}/common/src/HouseLocation.cpp
    ${PROJECT_SOURCE_DIR}/VacuumHouse.cpp
    ${PARENT_DIR}/common/src/Coordinate.cpp
    ${PROJECT_SOURCE_DIR}/test/VacuumHouseTest.cpp
  )
  add_gtest_executable(
    VacuumParserTest
    ${PROJECT_SOURCE_DIR}/VacuumParser.cpp
    ${PARENT_DIR}/common/src/HouseLocation.cpp
    ${PROJECT_SOURCE_DIR}/VacuumHouse.cpp
    ${PROJECT_SOURCE_DIR}/MeteredVacuumBattery.cpp
    ${PARENT_DIR}/common/src/Coordinate.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecordStep.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecord.cpp
    ${PROJECT_SOURCE_DIR}/test/VacuumParserTest.cpp
  )
  add_gtest_executable(
    BFSCleaingAfterMappingAlgorithmTest
    ${PROJECT_SOURCE_DIR}/VacuumParser.cpp
    ${PARENT_DIR}/common/src/HouseLocation.cpp
    ${PROJECT_SOURCE_DIR}/VacuumHouse.cpp
    ${PROJECT_SOURCE_DIR}/MeteredVacuumBattery.cpp
    ${PARENT_DIR}/common/src/Coordinate.cpp
    ${PROJECT_SOURCE_DIR}/VacuumSimulator.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecordStep.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecord.cpp
    ${PROJECT_SOURCE_DIR}/test/BFSCleaingAfterMappingAlgorithmTest.cpp
    ${PROJECT_SOURCE_DIR}/AlgorithmRegistrar.cpp
  )
  add_gtest_executable(
    BFSSimultaneousMappingAndCleaningAlgorithmTest
    ${PROJECT_SOURCE_DIR}/VacuumParser.cpp
    ${PARENT_DIR}/common/src/HouseLocation.cpp
    ${PROJECT_SOURCE_DIR}/VacuumHouse.cpp
    ${PROJECT_SOURCE_DIR}/MeteredVacuumBattery.cpp
    ${PARENT_DIR}/common/src/Coordinate.cpp
    ${PROJECT_SOURCE_DIR}/VacuumSimulator.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecordStep.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecord.cpp
    ${PROJECT_SOURCE_DIR}/test/BFSSimultaneousMappingAndCleaningAlgorithmTest.cpp
    ${PROJECT_SOURCE_DIR}/AlgorithmRegistrar.cpp
  )

  target_link_libraries(BFSSimultaneousMappingAndCleaningAlgorithmTest Boost::filesystem Boost::program_options Boost::thread)

  add_gtest_executable(
    BatchVacuumSimulatorTest
    ${PROJECT_SOURCE_DIR}/VacuumParser.cpp
    ${PARENT_DIR}/common/src/HouseLocation.cpp
    ${PROJECT_SOURCE_DIR}/VacuumHouse.cpp
    ${PROJECT_SOURCE_DIR}/MeteredVacuumBattery.cpp
    ${PARENT_DIR}/common/src/Coordinate.cpp
    ${PROJECT_SOURCE_DIR}/VacuumSimulator.cpp
    ${PROJECT_SOURCE_DIR}/SimulationArguments.cpp
    ${PROJECT_SOURCE_DIR}/BatchVacuumSimulator.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecordStep.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecord.cpp
    ${PROJECT_SOURCE_DIR}/test/BatchVacuumSimulatorTest.cpp
    ${PROJECT_SOURCE_DIR}/AlgorithmRegistrar.cpp
  )
  target_link_libraries(BatchVacuumSimulatorTest Boost::filesystem Boost::program_options Boost::thread)
  
  add_gtest_executable(
    VacuumSimulatorTest
    ${PROJECT_SOURCE_DIR}/VacuumSimulator.cpp
    ${PROJECT_SOURCE_DIR}/VacuumParser.cpp
    ${PARENT_DIR}/common/src/HouseLocation.cpp
    ${PROJECT_SOURCE_DIR}/VacuumHouse.cpp
    ${PROJECT_SOURCE_DIR}/MeteredVacuumBattery.cpp
    ${PARENT_DIR}/common/src/Coordinate.cpp
    ${PROJECT_SOURCE_DIR}/BatchVacuumSimulator.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecordStep.cpp
    ${PROJECT_SOURCE_DIR}/CleaningRecord.cpp
    ${PROJECT_SOURCE_DIR}/AlgorithmRegistrar.cpp
    ${PROJECT_SOURCE_DIR}/test/VacuumSimulatorTest.cpp
  )
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Werror -pedantic -rdynamic")
endif()

add_executable(myrobot 
  ${PROJECT_SOURCE_DIR}/main.cpp
  ${PROJECT_SOURCE_DIR}/CleaningRecord.cpp
  ${PROJECT_SOURCE_DIR}/CleaningRecordStep.cpp
  ${PARENT_DIR}/common/src/Coordinate.cpp
  ${PARENT_DIR}/common/src/HouseLocation.cpp
  ${PROJECT_SOURCE_DIR}/MeteredVacuumBattery.cpp
  ${PROJECT_SOURCE_DIR}/VacuumHouse.cpp
  ${PROJECT_SOURCE_DIR}/VacuumParser.cpp
  ${PROJECT_SOURCE_DIR}/VacuumSimulator.cpp
  ${PROJECT_SOURCE_DIR}/AlgorithmRegistrar.cpp
  ${PROJECT_SOURCE_DIR}/SimulationArguments.cpp
  ${PROJECT_SOURCE_DIR}/BatchVacuumSimulator.cpp
)

target_link_libraries(myrobot PRIVATE Boost::filesystem
                                         Boost::program_options)
