#pragma once

#include <memory>
#include <string>
#include <vector>
#include "lexer.hpp"

namespace slim::ast
{
    class Node {
        public:
        Node(Token token);
        virtual std::string Debug() = 0;

        protected:
        Token token;
    };

    class Expr : public Node
    {
        public:
        using Node::Node;

        // Ensure destructor of subclass is called when deleted
        virtual ~Expr() = default;
    };

    class BinaryExpr : public Expr
    {
        public:
        BinaryExpr(Token token, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);
        std::string Debug() override;

        protected:
        std::unique_ptr<Expr> left, right;
    };

    class BooleanExpr : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;
    };

    class ComparisonExpr : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;
    };

    class ArithmeticExpr : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;
    };

    class IndexAccess : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;
        std::string Debug() override;
    };

    class UnaryExpr : public Expr
    {
        public:
        UnaryExpr(Token token, std::unique_ptr<Expr> operand);
        std::string Debug() override;

        private:
        std::unique_ptr<Expr> operand;
    };

    class Identifier : public Expr
    {
        public:
        using Expr::Expr;
        std::string Debug() override;
    };

    class NumericLiteral : public Expr
    {
        public:
        using Expr::Expr;
        std::string Debug() override;
    };

    class BooleanLiteral : public Expr
    {
        public:
        using Expr::Expr;
        std::string Debug() override;
    };

    class PropertyAccess : public Expr {
        public:
        PropertyAccess(Token token, std::unique_ptr<Expr> accessed, std::unique_ptr<Identifier> property);
        std::string Debug() override;

        private:
        std::unique_ptr<Expr> accessed;
        std::unique_ptr<Identifier> property;
    };

    class FunctionCall : public Expr {
        public:
        FunctionCall(Token token, std::unique_ptr<Expr> fn, std::vector<std::unique_ptr<Expr>> args);
        std::string Debug() override;

        private:
        std::unique_ptr<Expr> fn;
        std::vector<std::unique_ptr<Expr>> args;
    };
}
