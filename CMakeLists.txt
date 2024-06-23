cmake_minimum_required(VERSION 3.20.0)
project(kleardb VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(fmt)

add_executable(kleardb main.cpp)
target_link_libraries(kleardb fmt::fmt)