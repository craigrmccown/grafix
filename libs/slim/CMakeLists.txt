# Source
add_library(slim STATIC
    src/slim/lexer.cpp
    src/slim/regex.cpp
)
target_include_directories(slim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Tests
add_executable(slim_test
    src/slim/lexer.test.cpp
    src/slim/regex.test.cpp
    src/slim/utf8.test.cpp
)
target_link_libraries(slim_test PRIVATE Catch2WithMain testutils slim)
catch_discover_tests(slim_test)
