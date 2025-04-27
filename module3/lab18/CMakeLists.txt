cmake_minimum_required(VERSION 3.10)
project(tcp_server C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(tcp_server
    server.c
)

install(TARGETS tcp_server
    RUNTIME DESTINATION bin
)
