cmake_minimum_required(VERSION 3.26)

project(path-tracer LANGUAGES CXX CUDA)

option(BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 86)

if(BUILD_TESTS)
    enable_testing()
endif()

add_executable(main src/main.cpp)

add_subdirectory(src/http-server)
add_subdirectory(src/udp-server)
add_subdirectory(src/path-tracer)

add_subdirectory(lib)
