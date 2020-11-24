cmake_minimum_required(VERSION 3.15)
project(crack C)

add_compile_options("/Os")
add_compile_options("/Oy")
add_compile_options("/Gy")

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES src/main.c)

add_subdirectory(src/console)
add_subdirectory(src/adt)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} console adt winmm.lib)
