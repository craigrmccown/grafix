#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "tokens.hpp"

namespace slim
{
    Parser::Parser(TokenIter &tokens) : tokens(tokens)
    {
        tokens.Next(current);
    }

    void Parser::Parse()
    {
        while (current.i != EOF)
        {
            if (is(TokenType::KeywordProperty) || is(TokenType::TagIdentifier))
            {
                pPropertyDecl();
            }
            else if (is(TokenType::KeywordFeature))
            {
                pFeatureBlock();
            }
            else if (is(TokenType::KeywordShared))
            {
                pSharedDecl();
            }
            else if (is(TokenType::KeywordShader))
            {
                pShaderBlock();
            }
            else
            {
                fail("Unexpected token: " + std::to_string(current.i));
            }
        }
    }

    void Parser::fail(const std::string &message) const
    {
        std::stringstream ss;
        ss
            << "Error at " << current.line << ":" << current.col << ": "
            << current.ToString() << "\n"
            << "  " << message;
        throw std::runtime_error(ss.str());
    }

    bool Parser::is(TokenType type) const
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


    Token Parser::expect(TokenType type)
    {
        if (!is(type))
        {
            fail(
                "Expected token " +
                std::to_string(type) +
                ", got " +
                std::to_string(current.i)
            );
        }

        return advance();
    }

    std::unique_ptr<ast::Expr> Parser::ParseExpression()
    {
        return pAssignmentExpr();
    }

    std::unique_ptr<ast::Expr> Parser::pAssignmentExpr()
    {
        std::unique_ptr<ast::Expr> expr = pOrExpr();

        while (is(TokenType::OpAssign))
        {
            Token op = advance();
            expr = std::make_unique<ast::AssignmentExpr>(op, std::move(expr), pOrExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pOrExpr()
    {
        std::unique_ptr<ast::Expr> expr = pAndExpr();

        while (is(TokenType::OpOr))
        {
            Token op = advance();
            expr = std::make_unique<ast::BooleanExpr>(op, std::move(expr), pAndExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pAndExpr()
    {
        std::unique_ptr<ast::Expr> expr = pEqualityExpr();

        while (is(TokenType::OpAnd))
        {
            Token op = advance();
            expr = std::make_unique<ast::BooleanExpr>(op, std::move(expr), pEqualityExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pEqualityExpr()
    {
        std::unique_ptr<ast::Expr> expr = pComparisonExpr();

        while (is(TokenType::OpEq) || is(TokenType::OpNeq))
        {
            Token op = advance();
            expr = std::make_unique<ast::ComparisonExpr>(op, std::move(expr), pComparisonExpr());
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
            Token op = advance();
            expr = std::make_unique<ast::ComparisonExpr>(op, std::move(expr), pAddExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pAddExpr()
    {
        std::unique_ptr<ast::Expr> expr = pMulExpr();

        while (is(TokenType::OpAdd) || is(TokenType::OpSub))
        {
            Token op = advance();
            expr = std::make_unique<ast::ArithmeticExpr>(op, std::move(expr), pMulExpr());
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
            Token op = advance();
            expr = std::make_unique<ast::ArithmeticExpr>(op, std::move(expr), pPrefixExpr());
        }

        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::pPrefixExpr()
    {
        if (is(TokenType::OpSub) || is(TokenType::OpBang))
        {
            Token op = advance();
            return std::make_unique<ast::UnaryExpr>(op, pPostfixExpr());
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
                Token open = advance();
                expr = std::make_unique<ast::IndexAccess>(open, std::move(expr), ParseExpression());
                expect(TokenType::CloseBracket);
            }
            else if (is(TokenType::OpenParen))
            {
                Token open = advance();
                expr = std::make_unique<ast::FunctionCall>(open, std::move(expr), pArgList());
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

    std::unique_ptr<ast::Statement> Parser::pStatement()
    {
        if (is(TokenType::DataType)) return pDeclStat();
        else if (is(TokenType::KeywordRequire)) return pRequireBlock();
        else if (is(TokenType::KeywordReturn)) return pReturnStat();
        else return pExprStat();
    }

    std::unique_ptr<ast::ReturnStat> Parser::pReturnStat()
    {
        Token start = expect(TokenType::KeywordReturn);
        std::unique_ptr<ast::Expr> expr = ParseExpression();
        expect(TokenType::Semicolon);
        return std::make_unique<ast::ReturnStat>(start, std::move(expr));
    }

    std::unique_ptr<ast::ExprStat> Parser::pExprStat()
    {
        Token start = current;
        std::unique_ptr<ast::Expr> expr = ParseExpression();
        expect(TokenType::Semicolon);
        return std::make_unique<ast::ExprStat>(start, std::move(expr));
    }

    std::unique_ptr<ast::DeclStat> Parser::pDeclStat()
    {
        Token type = expect(TokenType::DataType);
        Token identifier = expect(TokenType::Identifier);

        // Optionally parse default assignment
        std::unique_ptr<ast::Expr> expr = nullptr;
        if (check(TokenType::OpAssign))
        {
            expr = ParseExpression();
        }

        expect(TokenType::Semicolon);

        return std::make_unique<ast::DeclStat>(
            type,
            std::make_unique<ast::DataType>(type),
            std::make_unique<ast::Identifier>(identifier),
            std::move(expr)
        );
    }

    std::unique_ptr<ast::Tag> Parser::pTag()
    {
        assert(is(TokenType::TagIdentifier));
        Token tok = advance();

        if (is(TokenType::StringLiteral))
        {
            return std::make_unique<ast::Tag>(tok, std::make_unique<ast::StringLiteral>(advance()));
        }

        return std::make_unique<ast::Tag>(tok);
    }

    std::vector<std::unique_ptr<ast::Tag>> Parser::pTagList()
    {
        std::vector<std::unique_ptr<ast::Tag>> tags;

        while (is(TokenType::TagIdentifier))
        {
            tags.push_back(pTag());
        }

        return tags;
    }

    std::unique_ptr<ast::PropertyDecl> Parser::pPropertyDecl()
    {
        // The token marking this property declaration will either be a tag
        // identifier or the property keyword if no tags are present.
        Token start = current;

        // First parse tags, if any
        std::vector<std::unique_ptr<ast::Tag>> tags = pTagList();

        // Keyword is always required
        expect(TokenType::KeywordProperty);

        // The rest is identical to a declaration statement
        return std::make_unique<ast::PropertyDecl>(
            start,
            std::move(tags),
            pDeclStat()
        );
    }

    std::unique_ptr<ast::SharedDecl> Parser::pSharedDecl()
    {
        // Shared declarations do not allow initializers
        Token start = expect(TokenType::KeywordShared);
        Token type = expect(TokenType::DataType);
        Token identifier = expect(TokenType::Identifier);
        expect(TokenType::Semicolon);

        return std::make_unique<ast::SharedDecl>(
            start,
            std::make_unique<ast::DataType>(std::move(type)),
            std::make_unique<ast::Identifier>(std::move(identifier))
        );
    }

    std::unique_ptr<ast::FeatureBlock> Parser::pFeatureBlock()
    {
        Token start = expect(TokenType::KeywordFeature);
        Token identifier = expect(TokenType::Identifier);
        expect(TokenType::OpenBrace);

        std::vector<std::unique_ptr<ast::PropertyDecl>> decls;

        while (!check(TokenType::CloseBrace))
        {
            decls.push_back(pPropertyDecl());
        }

        return std::make_unique<ast::FeatureBlock>(
            start,
            std::make_unique<ast::Identifier>(identifier),
            std::move(decls)
        );
    }

    std::unique_ptr<ast::ShaderBlock> Parser::pShaderBlock()
    {
        Token start = expect(TokenType::KeywordShader);
        Token type = expect(TokenType::ShaderType);

        // Enforced by pattern definition, check for debug purposes
        assert(type.ToString() == "fragment" || type.ToString() == "vertex");

        expect(TokenType::OpenBrace);

        std::vector<std::unique_ptr<ast::Statement>> stats;

        while (!check(TokenType::CloseBrace))
        {
            stats.push_back(pStatement());
        }

        return std::make_unique<ast::ShaderBlock>(
            start,
            type.ToString() == "vertex"
                ? ast::ShaderBlock::Vertex
                : ast::ShaderBlock::Fragment,
            std::move(stats)
        );
    }

    std::unique_ptr<ast::RequireBlock> Parser::pRequireBlock()
    {
        Token start = expect(TokenType::KeywordRequire);
        Token identifier = expect(TokenType::Identifier);
        expect(TokenType::OpenBrace);

        std::vector<std::unique_ptr<ast::Statement>> stats;

        while (!check(TokenType::CloseBrace))
        {
            stats.push_back(pStatement());
        }

        return std::make_unique<ast::RequireBlock>(
            start,
            std::make_unique<ast::Identifier>(identifier),
            std::move(stats)
        );
    }
}
