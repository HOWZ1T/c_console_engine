cmake_minimum_required(VERSION 3.15)
project(gol C)

add_compile_options("/Os")
add_compile_options("/Oy")
add_compile_options("/Gy")

set(CMAKE_C_STANDARD 99)
set(SOURCE_FILES gol.c)

include_directories(../../include)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} console)
