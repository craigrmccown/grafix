add_library(testutils STATIC src/testutils/glm.cpp)
target_include_directories(testutils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(testutils PRIVATE glm)
