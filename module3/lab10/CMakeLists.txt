cmake_minimum_required(VERSION 3.10)
project(PosixSync C)

set(CMAKE_C_STANDARD 11)

# Находим библиотеку rt для POSIX семафоров
find_library(RT_LIBRARY rt)
if(NOT RT_LIBRARY)
    message(FATAL_ERROR "Библиотека rt не найдена (требуется для POSIX семафоров)")
endif()

add_executable(posix_sync lab10.c)
target_link_libraries(posix_sync ${RT_LIBRARY})
