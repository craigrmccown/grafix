add_library(core STATIC
    src/core/camera.cpp
    src/core/clock.cpp
    src/core/controls.cpp
    src/core/image.cpp
    src/core/model.cpp
    src/core/mouse.cpp
    src/core/point_light.cpp
    src/core/texture.cpp
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(core
    PRIVATE
        glad
        glfw
        glm::glm
        stb_image
)

if (APPLE)
    target_link_libraries(core
        PRIVATE
            "-framework Cocoa"
            "-framework IOKit")
endif()
