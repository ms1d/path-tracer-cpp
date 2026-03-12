cmake_minimum_required(VERSION 3.26)

add_library(test_lib src/testkernel.cu)
target_include_directories(test_lib PUBLIC include)

target_link_libraries(test_lib PUBLIC vec3)

set_target_properties(test_lib PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
)
