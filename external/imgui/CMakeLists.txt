# Dear Imgui is an immediate-mode GUI library useful for creating enging debug
# interfaces. To upgrade, copy *.{cpp,hpp} from the root of
# https://github.com/ocornut/imgui into this folder and update add_library
# sources.

add_library(imgui STATIC
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
