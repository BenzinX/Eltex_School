cmake_minimum_required(VERSION 3.10)
project(phonebook C)

# Настройка стандарта языка C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Создаем статическую библиотеку из telephone_directory.c
add_library(contactlist STATIC telephone_directory.c)

# Создаем исполняемый файл из main.c и линкуем его с библиотекой contactlist
add_executable(phonebook main.c)
target_link_libraries(phonebook PRIVATE contactlist)
