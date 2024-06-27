cmake_minimum_required(VERSION 3.20.0)
project(kleardb VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(WARNINGS -Wall -Wpedantic -Wextra -Wno-missing-field-initializers -Werror)
set(SANITIZERS -fsanitize=address -fno-omit-frame-pointer)

add_subdirectory(fmt)
add_subdirectory(docopt.cpp)

add_executable(kleardb main.cpp)
target_link_libraries(kleardb fmt::fmt)

add_executable(profile profile.cpp)
target_link_libraries(profile fmt::fmt docopt)
target_compile_options(profile PRIVATE "${WARNINGS}" -O2)
