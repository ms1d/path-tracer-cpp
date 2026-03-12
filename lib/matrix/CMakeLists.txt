cmake_minimum_required(VERSION 3.26)

add_library(matrix src/matrix.cu)
target_include_directories(matrix PUBLIC include)
target_link_libraries(matrix PUBLIC vec)
set_target_properties(matrix PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
)
