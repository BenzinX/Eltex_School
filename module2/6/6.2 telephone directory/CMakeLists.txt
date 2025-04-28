cmake_minimum_required(VERSION 3.10)
project(PhoneBook C)

set(CMAKE_C_STANDARD 11)

# Создание динамической библиотеки
add_library(contact_list SHARED
    contact_list.c
)

# Создание исполняемого файла
add_executable(phonebook
    main.c
)

# Линковка исполняемого файла с библиотекой
target_link_libraries(phonebook PRIVATE contact_list)

# Указание директорий для заголовочных файлов
target_include_directories(contact_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(phonebook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
