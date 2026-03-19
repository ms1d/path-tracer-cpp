cmake_minimum_required(VERSION 3.26)

add_executable(main main.cpp)

add_subdirectory(http-server)
add_subdirectory(udp-server)
add_subdirectory(path-tracer)
