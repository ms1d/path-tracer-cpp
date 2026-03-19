cmake_minimum_required(VERSION 3.26)

file(GLOB UDP_SRC *.cpp)
add_executable(udp-server ${UDP_SRC})

target_link_libraries(udp-server PRIVATE asio jsonlib)
target_compile_definitions(udp-server PRIVATE ASIO_STANDALONE)
