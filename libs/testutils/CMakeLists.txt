add_library(testutils STATIC src/testutils/dirgen.cpp src/testutils/glm.cpp)
target_include_directories(testutils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(testutils PRIVATE -Werror)
target_link_libraries(testutils PRIVATE Catch2 glm)
