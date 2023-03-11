cmake_minimum_required(VERSION 3.25.0)
set (CMAKE_CXX_STANDARD 17)

project(grafix
    VERSION 1.0
    DESCRIPTION "Graphics rendering engine"
    LANGUAGES CXX C)

# Update CMAKE_MODULE_PATH so that include(Catch) works and catch_discover_tests
# is available in all subdirectories
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/external/Catch2/extras)
include(CTest)
include(Catch)

add_subdirectory(tools)
add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(apps)
