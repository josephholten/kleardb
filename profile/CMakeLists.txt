add_executable(profile profile.cpp)
target_link_libraries(profile fmt::fmt docopt)
target_compile_options(profile PRIVATE "${WARNINGS}" -O2)