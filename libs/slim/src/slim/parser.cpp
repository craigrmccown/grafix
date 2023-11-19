#include <stdexcept>
#include <string>
#include "parser.hpp"
#include "lexer.hpp"

namespace slim
{
    Parser::Parser(TokenIter &tokens) : tokens(tokens) {
        tokens.Next(current);
    }

    void Parser::Parse() {
        ParseExpression();
    }

    bool Parser::check(TokenType type)
    {
        if (type == current.i)
        {
            if (!tokens.Next(current))
            {
                current = Token{ .i = EOF };
            }

            return true;
        }

        return false;
    }

    void Parser::expect(TokenType type)
    {
        if (!check(type))
        {
            throw std::runtime_error(
                "expected token " +
                std::to_string(type) +
                ", got " +
                std::to_string(current.i)
            );
        }
    }

    void Parser::ParseExpression()
    {
        pOrExpr();
    }

    void Parser::pOrExpr()
    {
        pAndExpr();

        while (check(TokenType::OpOr))
        {
            pAndExpr();
        }
    }

    void Parser::pAndExpr()
    {
        pEqualityExpr();

        while (check(TokenType::OpAnd))
        {
            pEqualityExpr();
        }
    }

    void Parser::pEqualityExpr()
    {
        pComparisonExpr();

        while (check(TokenType::OpEq) || check(TokenType::OpNeq))
        {
            pComparisonExpr();
        }
    }

    void Parser::pComparisonExpr()
    {
        pAddExpr();

        while (
            check(TokenType::OpGt) ||
            check(TokenType::OpLt) ||
            check(TokenType::OpGe) ||
            check(TokenType::OpLe)
        )
        {
            pAddExpr();
        }
    }

    void Parser::pAddExpr()
    {
        pMulExpr();

        while (check(TokenType::OpAdd) || check(TokenType::OpSub))
        {
            pMulExpr();
        }
    }

    void Parser::pMulExpr()
    {
        pPrefixExpr();

        while (
            check(TokenType::OpMul) ||
            check(TokenType::OpDiv) ||
            check(TokenType::OpMod)
        )
        {
            pPrefixExpr();
        }
    }

    void Parser::pPrefixExpr()
    {
        if (check(TokenType::OpSub) || check(TokenType::OpBang))
        {
            // Simply allow this for now
        }

        pPostfixExpr();
    }

    void Parser::pPostfixExpr()
    {
        pValueExpr();

        while (true)
        {
            if (check(TokenType::OpenBracket))
            {
                // Index expression
                ParseExpression();
                expect(TokenType::CloseBracket);
            }
            else if (check(TokenType::OpenParen))
            {
                // Function call
                pArgList();
            }
            else if (check(TokenType::Dot))
            {
                // Property access
                expect(TokenType::Identifier);
            }
            else
            {
                break;
            }
        }
    }

    void Parser::pArgList()
    {
        if (check(TokenType::CloseParen))
        {
            // No arguments
            return;
        }

        ParseExpression();

        while (check(TokenType::Comma))
        {
            ParseExpression();
        }

        expect(TokenType::CloseParen);
    }

    void Parser::pValueExpr()
    {
        if (check(TokenType::BoolLiteral) || check(TokenType::NumericLiteral) || check(TokenType::Identifier))
        {
            // Simply allow this for now
        }
        else if (check(TokenType::DataType))
        {
            expect(TokenType::OpenParen);
            pArgList();
        }
        else if (check(TokenType::OpenParen))
        {
            ParseExpression();
            expect(TokenType::CloseParen);
        }
        else
        {
            throw std::runtime_error("Unexpected token " + std::to_string(current.i));
        }
    }
}
