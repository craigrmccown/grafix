#pragma once

#include <memory>
#include <vector>
#include "ast.hpp"
#include "lexer.hpp"
#include "tokens.hpp"

namespace slim
{
    // TODO: Encapsulate lists within class definitions instead of returning
    // vectors directly
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
        Token expect(TokenType);

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
        std::unique_ptr<ast::Tag> pTag();
        std::vector<std::unique_ptr<ast::Tag>> pTagList();
        std::unique_ptr<ast::PropertyDecl> pPropertyDecl();
        std::unique_ptr<ast::SharedDecl> pSharedDecl();
        std::unique_ptr<ast::FeatureBlock> pFeatureBlock();
    };
}
