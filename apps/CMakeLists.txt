add_executable(demo demo.cpp)
target_compile_options(demo PRIVATE -Werror)
target_link_libraries(demo PRIVATE core glad glfw glm imgui orc)
