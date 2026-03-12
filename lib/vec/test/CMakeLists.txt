cmake_minimum_required(VERSION 3.26)

add_executable(test_vec test_vec.cpp test_vec.cu)
target_link_libraries(test_vec PRIVATE vec)

add_test(NAME test_vec COMMAND test_vec)
