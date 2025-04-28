cmake_minimum_required(VERSION 3.10)
project(PosixChat C)

set(CMAKE_C_STANDARD 11)

# Находим библиотеку rt для POSIX message queues
find_library(RT_LIBRARY rt)
if(NOT RT_LIBRARY)
    message(FATAL_ERROR "Библиотека rt не найдена (требуется для POSIX message queues)")
endif()

# Собираем обе программы чата
add_executable(posix_chat1 posix_chat1.c)
target_link_libraries(posix_chat1 ${RT_LIBRARY})

add_executable(posix_chat2 posix_chat2.c)
target_link_libraries(posix_chat2 ${RT_LIBRARY})
