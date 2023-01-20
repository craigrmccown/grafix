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
        stb/stb_image.cpp
)

target_link_libraries(grafix glfw3)
