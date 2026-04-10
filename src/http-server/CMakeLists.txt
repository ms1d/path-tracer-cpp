cmake_minimum_required(VERSION 3.26)

file(GLOB HTTP_SRC *.cpp)
add_executable(http-server ${HTTP_SRC})

find_package(ZLIB REQUIRED)

target_link_libraries(http-server PRIVATE httplib jsonlib asio ZLIB::ZLIB)
