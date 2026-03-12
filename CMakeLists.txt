cmake_minimum_required(VERSION 3.26)

project(cudaTest LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 86)

add_executable(cudaTest src/main.cpp)

add_subdirectory(lib)

target_link_libraries(cudaTest PRIVATE test_lib)
target_link_libraries(cudaTest PRIVATE ray)
target_link_libraries(cudaTest PRIVATE vec3)

option(BUILD_TESTS "Build test suite" ON)

if(BUILD_TESTS)
    enable_testing()

	file(GLOB TEST_DIRS "lib/*/test/CMakeLists.txt")

	foreach(TEST_FILE ${TEST_DIRS})
		get_filename_component(DIR ${TEST_FILE} DIRECTORY)
		add_subdirectory(${DIR})
	endforeach()

endif()
