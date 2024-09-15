cmake_minimum_required(VERSION 3.22.0)
project(kleardb VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(WARNING_FLAGS -Wall -Werror -Wpedantic -Wno-unused-function -fsanitize=address)
set(WARNING_LINK_FLAGS -fsanitize=address)
set(PERFORMANCE_FLAGS -Ofast -march=native -flto -fomit-frame-pointer)
set(DEBUG_FLAGS ${WARNING_FLAGS} -O0)
set(RELEASE_FLAGS ${WARNING_FLAGS} ${PERFORMANCE_FLAGS})

#add_subdirectory(fmt)
#add_subdirectory(profile)
find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(kleardb kleardb.cpp)
target_link_libraries(kleardb fmt::fmt)
target_compile_options(kleardb PRIVATE
  $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
  $<$<CONFIG:Release>:${PERFORMANCE_FLAGS}>
)
target_link_options(kleardb PRIVATE $<$<CONFIG:Debug>:${WARNING_LINK_FLAGS}>)

enable_testing()

add_executable(kleardb_test kleardb_test.cpp)
target_link_libraries(kleardb_test GTest::gtest_main fmt::fmt)
target_compile_options(kleardb_test PRIVATE ${WARNING_FLAGS})
target_link_options(kleardb_test PRIVATE ${WARNING_LINK_FLAGS})
include(GoogleTest)
gtest_discover_tests(kleardb_test)
add_test(NAME kleardb_test COMMAND ${CMAKE_BINARY_DIR}/kleardb_test)
