cmake_minimum_required(VERSION 3.10)
project(PosixSharedMemory C)

set(CMAKE_C_STANDARD 11)

# Находим библиотеку rt для POSIX shared memory
find_library(RT_LIBRARY rt)
if(NOT RT_LIBRARY)
    message(FATAL_ERROR "Библиотека rt не найдена (требуется для POSIX shared memory)")
endif()

add_executable(posix_shm lab13.c)
target_link_libraries(posix_shm ${RT_LIBRARY})
