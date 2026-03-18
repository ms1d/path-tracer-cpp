cmake_minimum_required(VERSION 3.26)

project(cudaTest LANGUAGES CXX CUDA)

option(BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 86)

if(BUILD_TESTS)
    enable_testing()
endif()

add_executable(cudaTest src/main.cpp)

#Asio
add_executable(asio_send src/asio_send_test.cpp)
add_executable(asio_receive src/asio_receive_test.cpp)

target_include_directories(asio_send PRIVATE lib/asio/include)
target_compile_definitions(asio_send PRIVATE ASIO_STANDALONE)
target_include_directories(asio_receive PRIVATE lib/asio/include)
target_compile_definitions(asio_receive PRIVATE ASIO_STANDALONE)

add_subdirectory(lib)
