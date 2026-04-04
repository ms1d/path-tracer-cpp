cmake_minimum_required(VERSION 3.26)

file(GLOB PATH_SRC *.cpp)
add_executable(path-tracer ${PATH_SRC})

target_link_libraries(path-tracer PRIVATE common ray mat vec jsonlib asio)
