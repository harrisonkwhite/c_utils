cmake_minimum_required(VERSION 3.28)
project(c_utils C)

set(CMAKE_C_STANDARD 11)

find_package(OpenGL REQUIRED)

add_library(c_utils STATIC
    src/cu_mem.c
    src/cu_math.c
    src/cu_io.c
    src/cu_graphics.c

    external/glad/src/glad.c
    external/stb/src/stb.c
)

target_include_directories(c_utils PUBLIC
    include

    external/c_utils/include
    external/stb/include
    external/glad/include
)

target_link_libraries(c_utils PUBLIC
    OpenGL::GL
)

target_compile_definitions(c_utils PRIVATE
    _CRT_SECURE_NO_WARNINGS
)
