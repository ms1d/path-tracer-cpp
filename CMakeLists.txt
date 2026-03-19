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

add_subdirectory(src)
add_subdirectory(lib)
