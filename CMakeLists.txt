cmake_minimum_required(VERSION 3.26)

project(cudaTest LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 86)

add_executable(cudaTest src/main.cpp)

add_subdirectory(lib)

target_link_libraries(cudaTest PRIVATE test)
target_link_libraries(cudaTest PRIVATE ray)
target_link_libraries(cudaTest PRIVATE vec3)
