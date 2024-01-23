#include <fstream>
#include <string>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <testutils/dirgen.hpp>
#include "lexer.hpp"
#include "parser.hpp"
#include "tokens.hpp"
#include "utf8.hpp"

class TestTokenIter : public slim::TokenIter
{
    public:
    TestTokenIter(std::initializer_list<slim::Token> tokens) : tokens(tokens), i(-1) { }

    bool Next(slim::Token &token)
    {
        int iNext = i + 1;
        if (iNext >= tokens.size()) return false;
        i = iNext;
        token = tokens[i];
        return true;
    }

    private:
    std::vector<slim::Token> tokens;
    int i;
};

// TODO: Possibly move this util into utf8.hpp if generally useful elsewhere
std::vector<slim::utf8::Glyph> decodeString(std::string s)
{
    std::vector<slim::utf8::Glyph> decoded;
    slim::utf8::Decoder decoder(s.begin(), s.end());
    slim::utf8::Glyph g;

    while (decoder.Next(g))
    {
        decoded.push_back(g);
    }

    assert(!decoder.Err());
    return decoded;
}

slim::Token makeToken(slim::TokenType tt, std::string data)
{
    return {.i = tt, .data = decodeString(data)};
}

TEST_CASE("valid expressions", "[slim]")
{
    struct TestCase {
        TestTokenIter tokens;
        std::string debug;
    };

    TestCase tc = GENERATE(
        TestCase{
            .tokens = {makeToken(slim::TokenType::NumericLiteral, "10")},
            .debug = "i{10}",
        },
        TestCase {
            .tokens = {makeToken(slim::TokenType::BoolLiteral, "true")},
            .debug = "b{true}",
        },
        TestCase {
            .tokens = {makeToken(slim::TokenType::Identifier, "myVar")},
            .debug = "id{myVar}",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::BoolLiteral, "true"),
                makeToken(slim::TokenType::OpAnd, "&&"),
                makeToken(slim::TokenType::BoolLiteral, "false"),
            },
            .debug = "(&& b{true} b{false})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::BoolLiteral, "true"),
                makeToken(slim::TokenType::OpOr, "||"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::BoolLiteral, "false"),
                makeToken(slim::TokenType::OpAnd, "&&"),
                makeToken(slim::TokenType::BoolLiteral, "true"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(|| b{true} (&& b{false} b{true}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::OpBang, "!"),
                makeToken(slim::TokenType::BoolLiteral, "true"),
                makeToken(slim::TokenType::OpOr, "||"),
                makeToken(slim::TokenType::BoolLiteral, "false"),
            },
            .debug = "(|| (! b{true}) b{false})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::OpGt, ">"),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::OpOr, "||"),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::OpLe, "<="),
                makeToken(slim::TokenType::NumericLiteral, "4"),
            },
            .debug = "(|| (> i{1} i{2}) (<= i{3} i{4}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "5"),
                makeToken(slim::TokenType::OpEq, "=="),
                makeToken(slim::TokenType::NumericLiteral, "5"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpAnd, "&&"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "6"),
                makeToken(slim::TokenType::OpNeq, "!="),
                makeToken(slim::TokenType::NumericLiteral, "6"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(&& (== i{5} i{5}) (!= i{6} i{6}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "cond1"),
                makeToken(slim::TokenType::OpAnd, "&&"),
                makeToken(slim::TokenType::Identifier, "cond2"),
                makeToken(slim::TokenType::OpOr, "||"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::Identifier, "cond3"),
                makeToken(slim::TokenType::OpOr, "||"),
                makeToken(slim::TokenType::Identifier, "cond4"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpAnd, "&&"),
                makeToken(slim::TokenType::Identifier, "cond5"),
            },
            .debug = "(|| (&& id{cond1} id{cond2}) (&& (|| id{cond3} id{cond4}) id{cond5}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "2"),
            },
            .debug = "(+ i{3} i{2})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "8"),
                makeToken(slim::TokenType::OpDiv, "/"),
                makeToken(slim::TokenType::NumericLiteral, "3"),
            },
            .debug = "(/ i{8} i{3})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "5"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "10"),
                makeToken(slim::TokenType::OpDiv, "/"),
                makeToken(slim::TokenType::NumericLiteral, "3"),
            },
            .debug = "(+ i{5} (/ i{10} i{3}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "4"),
                makeToken(slim::TokenType::OpMul, "*"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpSub, "-"),
                makeToken(slim::TokenType::NumericLiteral, "7"),
            },
            .debug = "(- (* i{4} (+ i{2} i{3})) i{7})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::OpMul, "*"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "4"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpSub, "-"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "5"),
                makeToken(slim::TokenType::OpMul, "*"),
                makeToken(slim::TokenType::NumericLiteral, "6"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(- (* i{2} (+ i{3} i{4})) (* i{5} i{6}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::OpMul, "*"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "5"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpDiv, "/"),
                makeToken(slim::TokenType::NumericLiteral, "4"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "6"),
            },
            .debug = "(+ (/ (* i{3} (+ i{2} i{5})) i{4}) i{6})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "4"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::OpSub, "-"),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::OpMul, "*"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "5"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(- (+ i{4} i{3}) (* i{2} (+ i{1} i{5})))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "4"),
            },
            .debug = "(+ (+ (+ i{1} i{2}) i{3}) i{4})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "myVar"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::OpGt, ">"),
                makeToken(slim::TokenType::Identifier, "otherVar"),
                makeToken(slim::TokenType::OpAnd, "&&"),
                makeToken(slim::TokenType::Identifier, "myVar"),
                makeToken(slim::TokenType::OpMod, "%"),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::OpEq, "=="),
                makeToken(slim::TokenType::NumericLiteral, "0"),
            },
            .debug = "(&& (> (+ id{myVar} i{1}) id{otherVar}) (== (% id{myVar} i{2}) i{0}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "sqrt"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "16"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(id{sqrt} i{16})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "max"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "5"),
                makeToken(slim::TokenType::Comma, ","),
                makeToken(slim::TokenType::NumericLiteral, "10"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpSub, "-"),
                makeToken(slim::TokenType::Identifier, "min"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::Comma, ","),
                makeToken(slim::TokenType::NumericLiteral, "7"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(- (id{max} i{5} i{10}) (id{min} i{2} i{7}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "abs"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::OpSub, "-"),
                makeToken(slim::TokenType::NumericLiteral, "7"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpMul, "*"),
                makeToken(slim::TokenType::Identifier, "pow"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::Comma, ","),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(* (id{abs} (- i{7})) (id{pow} i{2} i{3}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "arr"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "0"),
                makeToken(slim::TokenType::CloseBracket, "]"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::Identifier, "arr"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::CloseBracket, "]"),
            },
            .debug = "(+ ([] id{arr} i{0}) ([] id{arr} i{1}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::Identifier, "matrix"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::CloseBracket, "]"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::CloseBracket, "]"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::Identifier, "matrix"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::CloseBracket, "]"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::CloseBracket, "]"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpMul, "*"),
                makeToken(slim::TokenType::Identifier, "matrix"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "0"),
                makeToken(slim::TokenType::CloseBracket, "]"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "0"),
                makeToken(slim::TokenType::CloseBracket, "]"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(* (+ ([] ([] id{matrix} i{1}) i{2}) ([] ([] id{matrix} i{2}) i{1})) ([] ([] id{matrix} i{0}) i{0}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "obj"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "property"),
            },
            .debug = "(. id{obj} id{property})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "obj"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "property"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "nested"),
            },
            .debug = "(. (. id{obj} id{property}) id{nested})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::Identifier, "obj"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "prop1"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::Identifier, "obj"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "prop2"),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpMul, "*"),
                makeToken(slim::TokenType::Identifier, "obj"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "prop3"),
            },
            .debug = "(* (+ (. id{obj} id{prop1}) (. id{obj} id{prop2})) (. id{obj} id{prop3}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "arr"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::CloseBracket, "]"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "foo"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::CloseParen, ")"),
                makeToken(slim::TokenType::OpenBracket, "["),
                makeToken(slim::TokenType::Identifier, "i"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::CloseBracket, "]"),
            },
            .debug = "([] ((. ([] id{arr} i{1}) id{foo})) (+ id{i} i{1}))",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::DataType, "vec3"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::Comma, ","),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::Comma, ","),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
            .debug = "(id{vec3} i{1} i{2} i{3})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "foo"),
                makeToken(slim::TokenType::OpAssign, "="),
                makeToken(slim::TokenType::NumericLiteral, "5"),
            },
            .debug = "(= id{foo} i{5})",
        },
        TestCase {
            .tokens = {
                makeToken(slim::TokenType::Identifier, "foo"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "bar"),
                makeToken(slim::TokenType::OpAssign, "="),
                makeToken(slim::TokenType::Identifier, "baz")
            },
            .debug = "(= (. id{foo} id{bar}) id{baz})",
        }
    );

    slim::Parser parser(tc.tokens);
    std::unique_ptr<slim::ast::Expr> expr = parser.ParseExpression();
    CHECK(tc.debug == expr->Debug());
}

TEST_CASE("invalid expressions", "[slim]")
{
    struct TestCase {
        TestTokenIter tokens;
    };

    TestCase tc = GENERATE(
        TestCase{
            .tokens = {
                makeToken(slim::TokenType::NumericLiteral, "5"),
                makeToken(slim::TokenType::OpAdd, "+"),
            },
        },
        TestCase{
            .tokens = {
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "3"),
                makeToken(slim::TokenType::OpAdd, "+"),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
        },
        TestCase{
            .tokens = {
                makeToken(slim::TokenType::Identifier, "myVar"),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Dot, "."),
                makeToken(slim::TokenType::Identifier, "property"),
            },
        },
        TestCase{
            .tokens = {
                makeToken(slim::TokenType::Identifier, "myVar"),
                makeToken(slim::TokenType::OpenBracket, "["),
            },
        },
        TestCase{
            .tokens = {
                makeToken(slim::TokenType::Identifier, "fn"),
                makeToken(slim::TokenType::OpenParen, "("),
                makeToken(slim::TokenType::NumericLiteral, "1"),
                makeToken(slim::TokenType::Comma, ","),
                makeToken(slim::TokenType::NumericLiteral, "2"),
                makeToken(slim::TokenType::Comma, ","),
                makeToken(slim::TokenType::CloseParen, ")"),
            },
        }
    );

    slim::Parser parser(tc.tokens);
    CHECK_THROWS(parser.ParseExpression());
}

TEST_CASE("parse valid programs", "[slim]")
{
    auto entry = GENERATE(testutils::dir("libs/slim/test/parse/pass"));
    REQUIRE(entry.is_regular_file());

    std::ifstream ifs(entry.path(), std::ios::binary);
    ifs >> std::noskipws;

    REQUIRE(ifs.is_open());

    slim::Lexer lex(
        slim::patterns,
        std::istream_iterator<char>(ifs),
        std::istream_iterator<char>()
    );

    slim::Parser parser(lex);
    CHECK_NOTHROW(parser.Parse());
}
