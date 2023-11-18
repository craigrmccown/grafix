#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <vector>
#include "lexer.hpp"
#include "parser.hpp"

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


TEST_CASE("valid expressions", "[slim]")
{
    struct TestCase {
        TestTokenIter tokens;
    };

    std::vector<TestCase> testCases{
        // e.g. 10
        TestCase{
            .tokens = { { .i = slim::TokenType::NumericLiteral} },
        },

        // e.g. true"
        TestCase {
            .tokens = { { .i = slim::TokenType::BoolLiteral} },
        },

        // "e.g. myVar"
        TestCase {
            .tokens = { { .i = slim::TokenType::Identifier} },
        },

        // e.g. true && false
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::BoolLiteral},
                slim::Token{.i = slim::TokenType::OpAnd},
                slim::Token{.i = slim::TokenType::BoolLiteral},
            }
        },

        // e.g. true && false
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::BoolLiteral},
                slim::Token{.i = slim::TokenType::OpAnd},
                slim::Token{.i = slim::TokenType::BoolLiteral},
            }
        },

        // e.g. true || (false && true)
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::BoolLiteral},
                slim::Token{.i = slim::TokenType::OpOr},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::BoolLiteral},
                slim::Token{.i = slim::TokenType::OpAnd},
                slim::Token{.i = slim::TokenType::BoolLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
            }
        },

        // e.g. !true || false
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::OpBang},
                slim::Token{.i = slim::TokenType::BoolLiteral},
                slim::Token{.i = slim::TokenType::OpOr},
                slim::Token{.i = slim::TokenType::BoolLiteral},
            }
        },

        // e.g. 1 > 2 || 3 <= 4
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpGt},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpOr},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpLe},
                slim::Token{.i = slim::TokenType::NumericLiteral},
            }
        },

        // e.g. (5 == 5) && !(6 != 6)

        // e.g. cond1 && cond2 || (cond3 || cond4) && cond5
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpAnd},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpOr},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpOr},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpAnd},
                slim::Token{.i = slim::TokenType::Identifier},
            }
        },

        // e.g. 3 + 2
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
            }
        },

        // e.g. 8 / 3
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpDiv},
                slim::Token{.i = slim::TokenType::NumericLiteral},
            }
        },

        // e.g. 5 + 10 / 3
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpDiv},
                slim::Token{.i = slim::TokenType::NumericLiteral},
            }
        },

        // e.g. 4 * (2 + 3) - 7
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpMul},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpSub},
                slim::Token{.i = slim::TokenType::NumericLiteral},
            }
        },

        // e.g. 2 * (3 + 4) - (5 * 6)
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpMul},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpSub},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpMul},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
            }
        },

        // e.g. (3 * (2 + 5)) / 4 + 6
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpMul},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpDiv},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
            }
        },

        // e.g. 4 + 3 - 2 * (1 + 5)
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpSub},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpMul},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
            }
        },

        // e.g. myVar + 1 > otherVar && myVar % 2 == 0
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpGt},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpAnd},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpMod},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::OpEq},
                slim::Token{.i = slim::TokenType::NumericLiteral},
            }
        },

        // e.g. sqrt(16)
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
            }
        },

        // e.g. max(5, 10) - min(2, 7)
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::Comma},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpSub},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::Comma},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
            }
        },

        // e.g. abs(-7) * pow(2, 3)
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::OpSub},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpMul},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::Comma},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
            }
        },

        // e.g. array[0] + array[1]
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
            }
        },

        // e.g. (matrix[1][2] + matrix[2][1]) * matrix[0][0]
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpMul},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
                slim::Token{.i = slim::TokenType::CloseParen},
            }
        },

        // e.g. obj.property
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::Dot},
                slim::Token{.i = slim::TokenType::Identifier},
            }
        },

        // e.g. obj.property.nested
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::Dot},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::Dot},
                slim::Token{.i = slim::TokenType::Identifier},
            }
        },

        // e.g. (obj.prop1 + obj.prop2) * obj.prop3
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::Dot},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::Dot},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpMul},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::Dot},
                slim::Token{.i = slim::TokenType::Identifier},
            }
        },

        // e.g. arr[1].foo()[i + 1]
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
                slim::Token{.i = slim::TokenType::Dot},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::CloseParen},
                slim::Token{.i = slim::TokenType::OpenBracket},
                slim::Token{.i = slim::TokenType::Identifier},
                slim::Token{.i = slim::TokenType::OpAdd},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseBracket},
            }
        },

        // e.g. vec3(1, 2, 3)
        TestCase {
            .tokens = {
                slim::Token{.i = slim::TokenType::DataType},
                slim::Token{.i = slim::TokenType::OpenParen},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::Comma},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::Comma},
                slim::Token{.i = slim::TokenType::NumericLiteral},
                slim::Token{.i = slim::TokenType::CloseParen},
            }
        }
    };

    for (TestCase &tc : testCases)
    {
        slim::Parser parser(tc.tokens);
        CHECK_NOTHROW(parser.Parse());
    }
}
