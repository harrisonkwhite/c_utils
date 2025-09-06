cmake_minimum_required(VERSION 3.28)

project(cpp_utils CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cpp_utils STATIC
    src/cu_mem.cpp

    include/cu.h
    include/cu_math.h
    include/cu_mem.h
)

target_include_directories(cpp_utils PUBLIC
    include
)
