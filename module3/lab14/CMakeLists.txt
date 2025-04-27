cmake_minimum_required(VERSION 3.10)
project(UDP_Chat C)

set(CMAKE_C_STANDARD 11)

# Клиент
add_executable(udp_client
    client.c
)

# Сервер
add_executable(udp_server
    server.c
)

# Для Linux добавляем необходимые библиотеки
if(UNIX AND NOT APPLE)
    target_link_libraries(udp_client rt)
    target_link_libraries(udp_server rt)
endif()
