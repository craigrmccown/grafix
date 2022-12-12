cmake_minimum_required(VERSION 3.25.0)
set (CMAKE_CXX_STANDARD 17)

project(Grafix
    VERSION 1.0
    DESCRIPTION "Graphics rendering engine"
    LANGUAGES CXX C)

link_directories(/usr/local/lib)
include_directories(/usr/local/include ./include)

add_executable(grafix
    src/camera.cpp
    src/clock.cpp
    src/image.cpp
    src/main.cpp
    src/model.cpp
    src/mouse.cpp
    src/shader.cpp
    src/texture.cpp
    src/glad/glad.c
    src/stb/stb_image.cpp)

if (APPLE)
    target_link_libraries(grafix
        "-framework Cocoa"
        "-framework IOKit")
endif()

target_link_libraries(grafix glfw3)
