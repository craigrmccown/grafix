add_executable(grafix "")

target_sources(grafix
    PRIVATE
        camera.cpp
        clock.cpp
        controls.cpp
        image.cpp
        main.cpp
        model.cpp
        mouse.cpp
        point_light.cpp
        shader.cpp
        texture.cpp
        glad/glad.c
)

target_link_libraries(grafix glfw3 stb_image)
if (APPLE)
    target_link_libraries(grafix
        "-framework Cocoa"
        "-framework IOKit")
endif()
