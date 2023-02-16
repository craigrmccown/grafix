# Glad is a library that maps the OpenGL API to function pointers that can be
# called at runtime. Generated from https://glad.dav1d.de/.

add_library(glad STATIC glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
