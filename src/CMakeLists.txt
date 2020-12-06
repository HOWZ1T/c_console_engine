cmake_minimum_required(VERSION 3.15)
project(console C)

add_compile_options("/Os")
add_compile_options("/Oy")
add_compile_options("/Gy")

set(CMAKE_C_STANDARD 99)

set(LIB_HEADERS ../include/console/console.h ../include/console/chars.h ../include/console/colors.h ../include/console/rect.h)
set(LIB_SOURCE console.c)
set(LIB_FILES ${LIB_HEADERS} ${LIB_SOURCE})

add_library(${PROJECT_NAME} ${LIB_FILES})
target_link_libraries(${PROJECT_NAME} adt)