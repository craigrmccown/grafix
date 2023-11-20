#pragma once

#include <memory>
#include <vector>
#include "ast.hpp"
#include "lexer.hpp"

namespace slim
{
    enum TokenType {
        Eof,
        OpOr,
        OpAnd,
        OpEq,
        OpNeq,
        OpGt,
        OpLt,
        OpGe,
        OpLe,
        OpAdd,
        OpSub,
        OpMul,
        OpDiv,
        OpMod,
        OpBang,
        OpenParen,
        CloseParen,
        OpenBracket,
        CloseBracket,
        Dot,
        BoolLiteral,
        NumericLiteral,
        Identifier,
        DataType,
        Comma,
    };

    class Parser
    {
        public:
        Parser(TokenIter &tokens);

        void Parse();
        std::unique_ptr<ast::Expr> ParseExpression();

        private:
        Token current;
        TokenIter &tokens;

        bool is(TokenType);
        Token advance();
        bool check(TokenType);
        void expect(TokenType);

        std::unique_ptr<ast::Expr> pOrExpr();
        std::unique_ptr<ast::Expr> pAndExpr();
        std::unique_ptr<ast::Expr> pEqualityExpr();
        std::unique_ptr<ast::Expr> pComparisonExpr();
        std::unique_ptr<ast::Expr> pAddExpr();
        std::unique_ptr<ast::Expr> pMulExpr();
        std::unique_ptr<ast::Expr> pPrefixExpr();
        std::unique_ptr<ast::Expr> pPostfixExpr();
        std::vector<std::unique_ptr<ast::Expr>> pArgList();
        std::unique_ptr<ast::Expr> pValueExpr();
    };
}
