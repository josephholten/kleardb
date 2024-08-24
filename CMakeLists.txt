cmake_minimum_required(VERSION 3.20.0)
project(kleardb VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(fmt)

add_executable(kleardb kleardb.cpp)
target_link_libraries(kleardb fmt::fmt)

enable_testing()

add_executable(kleardb_test kleardb_test.cpp)
target_link_libraries(kleardb_test GTest::gtest_main fmt::fmt)
include(GoogleTest)
gtest_discover_tests(kleardb_test)