#include <string>
#include <utility>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include "lexer.hpp"

TEST_CASE("simple language tokenization", "[slim]")
{
    std::vector<std::string> patterns {
        "func",                         // KEYWORD
        "return",                       // KEYWORD
        "(void|bool|int|float|string)", // TYPE
        "[a-zA-Z][a-zA-Z0-9_]*",        // IDENTIFIER
        "\\(",                          // OPEN_PAREN
        "\\)",                          // CLOSE_PAREN
        "{",                            // OPEN_CURLY
        "}",                            // CLOSE_CURLY
        ",",                            // COMMA
        ";",                            // SEMICOLON
        "\\+",                          // OP_ADD
        "-",                            // OP_SUB
        "\\*",                          // OP_MUL
        "/",                             // OP_DIV
    };

    std::string input = "func myfunc(int a, int b) { return a + b; }";

    std::vector<std::pair<int, std::string>> expected = {
        {0, "func"},
        {3, "myfunc"},
        {4, "("},
        {2, "int"},
        {3, "a"},
        {8, ","},
        {2, "int"},
        {3, "b"},
        {5, ")"},
        {6, "{"},
        {1, "return"},
        {3, "a"},
        {10, "+"},
        {3, "b"},
        {9, ";"},
        {7, "}"},
    };

    slim::Lexer lex(
        patterns,
        input.begin(),
        input.end()
    );

    int i = 0;
    slim::Token token;
    while (lex.Next(token))
    {
        CHECK(expected[i].first == token.i);
        CHECK(expected[i].second == token.ToString());
        i++;
    }

    CHECK_FALSE(lex.Next(token));
}
