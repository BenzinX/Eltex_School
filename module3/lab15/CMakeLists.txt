cmake_minimum_required(VERSION 3.10)
project(TCP_Calculator C)

set(CMAKE_C_STANDARD 11)

# Сервер
add_executable(tcp_server
    server.c
)

# Клиент
add_executable(tcp_client
    client.c
)

# Для Linux добавляем необходимые библиотеки
if(UNIX AND NOT APPLE)
    target_link_libraries(tcp_server rt)
    target_link_libraries(tcp_client rt)
endif()
