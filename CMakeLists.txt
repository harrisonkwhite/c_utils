cmake_minimum_required(VERSION 3.28)
project(c_utils C)

set(CMAKE_C_STANDARD 11)

add_library(c_utils STATIC
    src/mem.c
    src/io.c

    include/mem.h
    include/io.h
)

target_include_directories(c_utils PUBLIC
    include
)

target_compile_definitions(c_utils PRIVATE
    _CRT_SECURE_NO_WARNINGS
)
