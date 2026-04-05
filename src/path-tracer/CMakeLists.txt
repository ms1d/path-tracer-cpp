cmake_minimum_required(VERSION 3.26)

file(GLOB PATH_SRC src/*.cpp src/*.cu)
add_executable(path-tracer ${PATH_SRC})

target_include_directories(path-tracer PRIVATE include)

target_link_libraries(path-tracer PRIVATE common ray mat vec jsonlib asio)
