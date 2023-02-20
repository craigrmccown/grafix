add_library(orc STATIC
    src/orc/camera.cpp
    src/orc/light.cpp
    src/orc/obj.cpp
    src/orc/regular.cpp
    src/orc/scene_graph.cpp
)

target_include_directories(orc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(orc PRIVATE glm::glm)
