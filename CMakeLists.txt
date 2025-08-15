cmake_minimum_required(VERSION 3.28)
project(c_utils C)

set(CMAKE_C_STANDARD 11)

add_library(c_utils STATIC
    src/cu_mem.c
    src/cu_math.c
    src/cu_io.c

    include/cu.h
    include/cu_io.h
    include/cu_math.h
    include/cu_mem.h
)

target_include_directories(c_utils PUBLIC
    include
)

if (NOT MSVC)
    target_link_libraries(c_utils PUBLIC
        m
    )
endif()
