cmake_minimum_required(VERSION 3.10)
project(SystemVChat C)

set(CMAKE_C_STANDARD 11)

# Собираем обе программы чата
add_executable(chat1 chat1.c)
add_executable(chat2 chat2.c)
