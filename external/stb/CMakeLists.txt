# STB is a collection of header-only libraries. To upgrade, copy the desired
# headers from https://github.com/nothings/stb into this folder, then follow any
# instructions stated in the comment at the top of the copied header.

add_library(stb_image STATIC stb/stb_image.cpp)
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
