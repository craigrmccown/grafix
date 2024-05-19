# Source
add_library(slim STATIC
    src/slim/alphabet.cpp
    src/slim/ast.cpp
    src/slim/dfa.cpp
    src/slim/nfa.cpp
    src/slim/parser.cpp
    src/slim/regex.cpp
    src/slim/tokens.cpp
    src/slim/types.cpp
)
target_compile_options(slim PRIVATE -Werror)
target_include_directories(slim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Tests
add_executable(slim_test
    src/slim/alphabet.test.cpp
    src/slim/dfa.test.cpp
    src/slim/lexer.test.cpp
    src/slim/nfa.test.cpp
    src/slim/parser.test.cpp
    src/slim/regex.test.cpp
    src/slim/types.test.cpp
    src/slim/utf8.test.cpp
)
target_compile_options(slim_test PRIVATE -Werror)
target_link_libraries(slim_test PRIVATE Catch2WithMain testutils slim)
catch_discover_tests(slim_test)
