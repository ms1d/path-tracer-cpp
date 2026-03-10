cmake_minimum_required(VERSION 3.26)

add_library(vec3 src/vec3.cpp)
target_include_directories(vec3 PUBLIC include)
set_target_properties(vec3 PROPERTIES
	CUDA_SEPARABLE_COMPILATION ON
)
