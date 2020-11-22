cmake_minimum_required(VERSION 3.15)
project(crack C)

add_compile_options("/Os")
add_compile_options("/Oy")
add_compile_options("/Gy")

set(CMAKE_C_STANDARD 99)

set(LIB_HEADERS src/console/console.h src/console/chars.h src/console/colors.h)
set(LIB_SOURCE src/console/console.c)
set(LIB_FILES ${LIB_HEADERS} ${LIB_SOURCE})

set(SOURCE_FILES src/main.c)

add_library(Console ${LIB_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} Console winmm.lib)
