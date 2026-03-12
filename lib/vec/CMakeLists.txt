cmake_minimum_required(VERSION 3.26)

add_library(vec INTERFACE)
target_include_directories(vec INTERFACE include)

if (BUILD_TESTS)
	add_subdirectory(test)
endif()
