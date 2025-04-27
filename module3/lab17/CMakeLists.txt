cmake_minimum_required(VERSION 3.10)
project(udp_sniffer)

set(CMAKE_C_STANDARD 11)

# Альтернативный способ поиска pcap
find_library(PCAP_LIB pcap)
if(NOT PCAP_LIB)
    message(FATAL_ERROR "Библиотека libpcap не найдена. Установите через <sudo apt-get install libpcap-dev>")
endif()

add_executable(udp_sniffer
    sniffer.c
)

target_link_libraries(udp_sniffer
    ${PCAP_LIB}
)

target_compile_options(udp_sniffer PRIVATE
    -Wall
    -Wextra
)
