cmake_minimum_required(VERSION 3.10)
project(Module3Lab2 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Основная оболочка
add_executable(myshell shell.c)

# Утилита для конкатенации строк
add_executable(stringsum stringsum.c)

# Утилита для суммирования чисел
add_executable(sum sum.c)
