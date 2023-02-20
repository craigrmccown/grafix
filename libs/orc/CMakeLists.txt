# Source
add_library(orc STATIC
    src/orc/camera.cpp
    src/orc/light.cpp
    src/orc/obj.cpp
    src/orc/regular.cpp
    src/orc/scene_graph.cpp
)
set(LDEPS glm::glm)
target_include_directories(orc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(orc PRIVATE ${LDEPS})

# Tests
add_executable(orc_test src/orc/scene_graph.test.cpp)
target_link_libraries(orc_test PRIVATE Catch2::Catch2WithMain orc ${LDEPS})
catch_discover_tests(orc_test)
