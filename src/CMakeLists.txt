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
)

target_link_libraries(grafix
    PRIVATE
        glad
        glfw3
        glm::glm
        imgui
        stb_image
)

if (APPLE)
    target_link_libraries(grafix
        PRIVATE
            "-framework Cocoa"
            "-framework IOKit")
endif()
