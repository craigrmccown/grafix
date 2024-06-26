# Resources
# TODO: CMake scripts to DRY up embedded resource configuration
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/monochrome.frag.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/monochrome.frag.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/monochrome.frag
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/monochrome.frag
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/monochrome.vert.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/monochrome.vert.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/monochrome.vert
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/monochrome.vert
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/phong.frag.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/phong.frag.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/phong.frag
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/phong.frag
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/phong.vert.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/phong.vert.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/phong.vert
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/phong.vert
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/skybox.frag.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/skybox.frag.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/skybox.frag
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/skybox.frag
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/skybox.vert.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/skybox.vert.hpp
  COMMAND binembed orc::shaders ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/skybox.vert
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/orc/shaders/skybox.vert
)

# Source
add_library(orc STATIC
    src/orc/camera.cpp
    src/orc/cube.cpp
    src/orc/cubemap.cpp
    src/orc/image.cpp
    src/orc/light.cpp
    src/orc/mesh.cpp
    src/orc/model.cpp
    src/orc/node.cpp
    src/orc/object.cpp
    src/orc/scene.cpp
    src/orc/shader.cpp
    src/orc/skybox.cpp
    src/orc/stateful_visitor.cpp
    src/orc/texture.cpp
    src/orc/texture_2d.cpp

    # Embedded resources
    src/orc/shaders/monochrome.frag.cpp
    src/orc/shaders/monochrome.vert.cpp
    src/orc/shaders/phong.frag.cpp
    src/orc/shaders/phong.vert.cpp
    src/orc/shaders/skybox.frag.cpp
    src/orc/shaders/skybox.vert.cpp
)
target_include_directories(orc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(orc PRIVATE -Werror)
target_link_libraries(orc PRIVATE
    assimp
    glad
    glm::glm
    stb_image
)

# Tests
add_executable(orc_test
    test/main.cpp
    src/orc/camera.test.cpp
    src/orc/node.test.cpp
    src/orc/scene.test.cpp
    src/orc/shader.test.cpp
)
target_compile_options(orc_test PRIVATE -Werror)
target_link_libraries(orc_test PRIVATE
    Catch2::Catch2
    glad
    glfw
    glm::glm
    orc
    testutils
)
catch_discover_tests(orc_test)

# Examples
add_executable(basic_usage examples/basic_usage.cpp)
target_link_libraries(basic_usage PRIVATE
  glad
  glfw
  glm::glm
  orc
)
