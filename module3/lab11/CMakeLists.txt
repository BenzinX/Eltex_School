cmake_minimum_required(VERSION 3.10)
project(MultiProcessSync C)

set(CMAKE_C_STANDARD 11)

# Находим библиотеку rt для POSIX семафоров
find_library(RT_LIBRARY rt)
if(NOT RT_LIBRARY)
    message(FATAL_ERROR "Библиотека rt не найдена (требуется для POSIX семафоров)")
endif()

add_executable(multi_process_sync lab11.c)
target_link_libraries(multi_process_sync ${RT_LIBRARY})
