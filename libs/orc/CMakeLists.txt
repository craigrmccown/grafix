# Resources
# TODO: CMake scripts to DRY up embedded resource configuration
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/light.frag.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/light.frag.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/light.frag
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/light.frag
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/light.vert.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/light.vert.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/light.vert
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/light.vert
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/regular.frag.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/regular.frag.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/regular.frag
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/regular.frag
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/regular.vert.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/regular.vert.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/regular.vert
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/regular.vert
)

# Source
add_library(orc STATIC
    src/orc/camera.cpp
    src/orc/light.cpp
    src/orc/obj.cpp
    src/orc/regular.cpp
    src/orc/scene.cpp
    src/orc/shader.cpp
    src/orc/stateful_visitor.cpp

    # Embedded resources
    src/orc/shaders/light.frag.cpp
    src/orc/shaders/light.vert.cpp
    src/orc/shaders/regular.frag.cpp
    src/orc/shaders/regular.vert.cpp
)
set(LDEPS glad glm::glm)
target_include_directories(orc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(orc PRIVATE ${LDEPS})

# Tests
add_executable(orc_test
    test/main.cpp
    src/orc/camera.test.cpp
    src/orc/scene.test.cpp
)
target_link_libraries(orc_test PRIVATE
    Catch2::Catch2
    glfw
    orc
    ${LDEPS}
)
catch_discover_tests(orc_test)
