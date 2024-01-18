#include <string>
#include <tuple>
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
    CHECK(i == expected.size());
}

TEST_CASE("token line and column numbers", "[slim]")
{
    std::vector<std::string> patterns {"red", "green", "blue", ";"};
    std::string input = "red green;\nblue green red;\ngreen;";

    // {token, line, column}
    std::vector<std::tuple<int, int, int>> expected = {
        {0, 1, 0},
        {1, 1, 4},
        {3, 1, 9},
        {2, 2, 0},
        {1, 2, 5},
        {0, 2, 11},
        {3, 2, 14},
        {1, 3, 0},
        {3, 3, 5},
    };

    slim::Lexer lex(patterns, input.begin(), input.end());

    int i = 0;
    slim::Token token;
    while (lex.Next(token))
    {
        CHECK(std::get<0>(expected[i]) == token.i);
        CHECK(std::get<1>(expected[i]) == token.line);
        CHECK(std::get<2>(expected[i]) == token.col);
        i++;
    }

    CHECK_FALSE(lex.Next(token));
    CHECK(i == expected.size());
}

TEST_CASE("token pattern and input including whitespace", "[slim]")
{
    std::vector<std::string> patterns {"\".*\""};
    std::string input = "  \"hello, world!    :) \"";
    slim::Lexer lex(patterns, input.begin(), input.end());

    slim::Token token;
    REQUIRE(lex.Next(token));
    CHECK(token.i == 0);
    CHECK_FALSE(lex.Next(token));
}

TEST_CASE("unmatched characters", "[slim]")
{
    std::vector<std::string> patterns {"a", "b", "c", "e"};
    std::string input = "ab c  d  e";
    std::vector<int> expected = {0, 1, 2, -1, 3};
    slim::Lexer lex(patterns, input.begin(), input.end());

    slim::Token token;
    for (int i = 0; i < expected.size(); i++)
    {
        if (expected[i] == -1)
        {
            CHECK_THROWS(lex.Next(token));
        }
        else
        {
            REQUIRE(lex.Next(token));
            CHECK(token.i == expected[i]);
        }
    }

    CHECK_FALSE(lex.Next(token));
}
