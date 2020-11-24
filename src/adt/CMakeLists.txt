cmake_minimum_required(VERSION 3.15)
project(adt C)

add_compile_options("/Os")
add_compile_options("/Oy")
add_compile_options("/Gy")

set(CMAKE_C_STANDARD 99)

set(LIB_HEADERS vector.h)
set(LIB_SOURCE vector.c)
set(LIB_FILES ${LIB_HEADERS} ${LIB_SOURCE})

add_library(${PROJECT_NAME} ${LIB_FILES})
