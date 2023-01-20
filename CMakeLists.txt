cmake_minimum_required(VERSION 3.25.0)
set (CMAKE_CXX_STANDARD 17)

project(Grafix
    VERSION 1.0
    DESCRIPTION "Graphics rendering engine"
    LANGUAGES CXX C)

link_directories(/usr/local/lib)
include_directories(/usr/local/include ./include)

add_executable(grafix "")

add_subdirectory(src)

if (APPLE)
    target_link_libraries(grafix
        "-framework Cocoa"
        "-framework IOKit")
endif()
