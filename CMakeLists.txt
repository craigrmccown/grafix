cmake_minimum_required(VERSION 3.25.0)
set (CMAKE_CXX_STANDARD 17)

project(grafix
    VERSION 1.0
    DESCRIPTION "Graphics rendering engine"
    LANGUAGES CXX C)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)

add_subdirectory(external)
add_subdirectory(src)
