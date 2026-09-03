cmake_minimum_required(VERSION 3.20)
project(
  clihehe
  VERSION 0.1.0
  LANGUAGES CXX)

option(CLIHEHE_BUILD_TESTS "Tests." ${PROJECT_IS_TOP_LEVEL})

message(STATUS "Current C++ version: ${CMAKE_CXX_STANDARD}")

add_library(clihehe_header_only INTERFACE)
add_library(clihehe::clihehe-header-only ALIAS clihehe_header_only)

target_include_directories(
  clihehe_header_only
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

target_compile_definitions(clihehe_header_only INTERFACE CLIHEHE_HEADER_ONLY=1)

file(GLOB_RECURSE CLIHEHE_SOURCES "lib/*.cpp")

add_library(clihehe STATIC ${CLIHEHE_SOURCES})
add_library(clihehe::clihehe ALIAS clihehe)

target_include_directories(
  clihehe PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                 $<INSTALL_INTERFACE:include>)

if(CLIHEHE_BUILD_TESTS)
  include(FetchContent)

  message(STATUS "Test enabled!")
  option(CLIHEHE_TEST_FMT "Test {fmt}" OFF)

  if(CLIHEHE_TEST_FMT)

    FetchContent_Declare(
      fmt
      GIT_REPOSITORY https://github.com/fmtlib/fmt
      GIT_TAG        12.2.0)
    FetchContent_MakeAvailable(fmt)

  endif()

  enable_testing()

  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.18.0)
  FetchContent_MakeAvailable(googletest)

  file(GLOB_RECURSE CLIHEHE_TESTS_SOURCES "unittests/*.cpp")
  add_executable(clihehe_tests ${CLIHEHE_TESTS_SOURCES})

  if(CLIHEHE_TEST_FMT)
    target_link_libraries(clihehe_tests fmt::fmt)
    target_compile_definitions(clihehe_Tests PUBLIC NAO_CLIHH_FMT_FORMAT=1 NAO_CLIHH_FMT_PRINT=1)
  endif()

  target_link_libraries(clihehe_tests clihehe::clihehe-header-only)
  target_link_libraries(clihehe_tests GTest::gtest_main GTest::gmock)

  include(GoogleTest)
  gtest_discover_tests(clihehe_tests)

endif()
