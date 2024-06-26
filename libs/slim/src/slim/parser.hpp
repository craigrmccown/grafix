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

        std::unique_ptr<ast::Program> Parse();
        std::unique_ptr<ast::Expr> ParseExpression();

        private:
        Token current;
        Token next;
        TokenIter &tokens;

        void fail(const std::string &message) const;
        bool is(TokenType) const;
        Token advance();
        bool check(TokenType);
        Token expect(TokenType);
        bool peekIs(TokenType);

        std::unique_ptr<ast::Expr> pAssignmentExpr();
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
        std::unique_ptr<ast::Node> pStatement();
        std::unique_ptr<ast::ExprStat> pExprStat();
        std::unique_ptr<ast::ReturnStat> pReturnStat();
        std::unique_ptr<ast::DeclStat> pDeclStat();
        std::unique_ptr<ast::Tag> pTag();
        std::vector<std::unique_ptr<ast::Tag>> pTagList();
        std::unique_ptr<ast::PropertyDecl> pPropertyDecl();
        std::unique_ptr<ast::SharedDecl> pSharedDecl();
        std::unique_ptr<ast::FeatureBlock> pFeatureBlock();
        std::unique_ptr<ast::ShaderBlock> pShaderBlock();
        std::unique_ptr<ast::RequireBlock> pRequireBlock();
    };
}
