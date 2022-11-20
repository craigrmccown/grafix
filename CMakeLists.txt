cmake_minimum_required(VERSION 3.25.0)

project(Grafix
    VERSION 1.0
    DESCRIPTION "Graphics rendering engine"
    LANGUAGES CXX C)

link_directories(/usr/local/lib)
include_directories(/usr/local/include ./include)

add_executable(grafix
    src/main.cpp
    src/glad/glad.c)

if (APPLE)
    target_link_libraries(grafix
        "-framework Cocoa"
        "-framework IOKit")
endif()

target_link_libraries(grafix glfw3)
