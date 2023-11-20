#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "parser.hpp"
#include "ast.hpp"
#include "lexer.hpp"

namespace slim
{
    Parser::Parser(TokenIter &tokens) : tokens(tokens) {
        tokens.Next(current);
    }

    void Parser::Parse() {
        ParseExpression();
    }

    bool Parser::is(TokenType type)
    {
        return type == current.i;
    }

    Token Parser::advance()
    {
        Token tok = current;
        if (!tokens.Next(current))
        {
            current = Token{ .i = EOF };
        }
        return tok;
    }

    bool Parser::check(TokenType type)
    {
        if (!is(type)) return false;
        advance();
        return true;
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

    std::unique_ptr<ast::Expr> Parser::ParseExpression()
    {
        return pOrExpr();
    }

    std::unique_ptr<ast::Expr> Parser::pOrExpr()
    {
        std::unique_ptr<ast::Expr> expr = pAndExpr();

        while (is(TokenType::OpOr))
        {
            expr = std::make_unique<ast::BooleanExpr>(advance(), std::move(expr), pAndExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pAndExpr()
    {
        std::unique_ptr<ast::Expr> expr = pEqualityExpr();

        while (is(TokenType::OpAnd))
        {
            expr = std::make_unique<ast::BooleanExpr>(advance(), std::move(expr), pEqualityExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pEqualityExpr()
    {
        std::unique_ptr<ast::Expr> expr = pComparisonExpr();

        while (is(TokenType::OpEq) || is(TokenType::OpNeq))
        {
            expr = std::make_unique<ast::ComparisonExpr>(advance(), std::move(expr), pComparisonExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pComparisonExpr()
    {
        std::unique_ptr<ast::Expr> expr = pAddExpr();

        while (
            is(TokenType::OpGt) ||
            is(TokenType::OpLt) ||
            is(TokenType::OpGe) ||
            is(TokenType::OpLe)
        )
        {
            expr = std::make_unique<ast::ComparisonExpr>(advance(), std::move(expr), pAddExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pAddExpr()
    {
        std::unique_ptr<ast::Expr> expr = pMulExpr();

        while (is(TokenType::OpAdd) || is(TokenType::OpSub))
        {
            expr = std::make_unique<ast::ArithmeticExpr>(advance(), std::move(expr), pMulExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pMulExpr()
    {
        std::unique_ptr<ast::Expr> expr = pPrefixExpr();

        while (
            is(TokenType::OpMul) ||
            is(TokenType::OpDiv) ||
            is(TokenType::OpMod)
        )
        {
            expr = std::make_unique<ast::ArithmeticExpr>(advance(), std::move(expr), pPrefixExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pPrefixExpr()
    {
        if (is(TokenType::OpSub) || is(TokenType::OpBang))
        {
            return std::make_unique<ast::UnaryExpr>(advance(), pPostfixExpr());
        }

        return pPostfixExpr();
    }

    std::unique_ptr<ast::Expr> Parser::pPostfixExpr()
    {
        std::unique_ptr<ast::Expr> expr = pValueExpr();

        while (true)
        {
            if (is(TokenType::OpenBracket))
            {
                expr = std::make_unique<ast::IndexAccess>(advance(), std::move(expr), ParseExpression());
                expect(TokenType::CloseBracket);
            }
            else if (is(TokenType::OpenParen))
            {
                expr = std::make_unique<ast::FunctionCall>(advance(), std::move(expr), pArgList());
            }
            else if (is(TokenType::Dot))
            {
                Token dot = advance();
                Token identifier = current;
                expect(TokenType::Identifier);
                expr = std::make_unique<ast::PropertyAccess>(dot, std::move(expr), std::make_unique<ast::Identifier>(identifier));
            }
            else
            {
                break;
            }
        }

        return expr;
    }

    std::vector<std::unique_ptr<ast::Expr>> Parser::pArgList()
    {
        std::vector<std::unique_ptr<ast::Expr>> args;

        if (check(TokenType::CloseParen))
        {
            return args;
        }

        args.push_back(ParseExpression());

        while (check(TokenType::Comma))
        {
            args.push_back(ParseExpression());
        }

        expect(TokenType::CloseParen);
        return args;
    }

    std::unique_ptr<ast::Expr> Parser::pValueExpr()
    {
        std::unique_ptr<ast::Expr> expr;

        if (is(TokenType::BoolLiteral))
        {
            expr = std::make_unique<ast::BooleanLiteral>(advance());
        }
        else if (is(TokenType::NumericLiteral))
        {
            expr = std::make_unique<ast::NumericLiteral>(advance());
        }
        else if (is(TokenType::Identifier))
        {
            expr = std::make_unique<ast::Identifier>(advance());
        }
        else if (is(TokenType::DataType))
        {
            Token dataType = advance();
            Token openParen = current;
            expect(TokenType::OpenParen);

            // Coerce data type token into an identifier - built in data type
            // functions will be prepopulated into the global symbol table
            expr = std::make_unique<ast::FunctionCall>(openParen, std::make_unique<ast::Identifier>(dataType), pArgList());
        }
        else
        {
            expect(TokenType::OpenParen);
            expr = ParseExpression();
            expect(TokenType::CloseParen);
        }

        return expr;
    }
}
