#pragma once

#include <memory>
#include <string>
#include <vector>
#include "lexer.hpp"

namespace slim::ast
{
    class Node
    {
        public:
        Node(Token token);
        virtual std::string Debug() = 0;

        protected:
        Token token;
    };

    class Statement : public Node
    {
        public:
        using Node::Node;

        // Ensure destructor of subclass is called when deleted
        virtual ~Statement() = default;
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

    // As expressions can only produce numerical values, string literals do not
    // inherit from Expr. However, we still include a string literal type for
    // use outside of expressions, e.g. property tag metadata
    class StringLiteral : public Node
    {
        public:
        using Node::Node;
        std::string Debug() override;
    };

    class DataType : public Node
    {
        public:
        using Node::Node;
        std::string Debug() override;
    };

    class PropertyAccess : public Expr
    {
        public:
        PropertyAccess(Token token, std::unique_ptr<Expr> accessed, std::unique_ptr<Identifier> property);
        std::string Debug() override;

        private:
        std::unique_ptr<Expr> accessed;
        std::unique_ptr<Identifier> property;
    };

    class FunctionCall : public Expr
    {
        public:
        FunctionCall(Token token, std::unique_ptr<Expr> fn, std::vector<std::unique_ptr<Expr>> args);
        std::string Debug() override;

        private:
        std::unique_ptr<Expr> fn;
        std::vector<std::unique_ptr<Expr>> args;
    };

    class Tag : public Node
    {
        public:
        Tag(Token token, std::unique_ptr<StringLiteral> meta = nullptr);
        std::string Debug() override;

        private:
        std::unique_ptr<StringLiteral> meta;
    };

    class PropertyDecl: public Statement
    {
        public:
        PropertyDecl(
            Token token,
            std::vector<std::unique_ptr<Tag>> tags,
            std::unique_ptr<DataType> type,
            std::unique_ptr<Identifier> identifier,
            std::unique_ptr<Expr> initializer = nullptr
        );
        std::string Debug() override;

        private:
        std::vector<std::unique_ptr<Tag>> tags;
        std::unique_ptr<DataType> type;
        std::unique_ptr<Identifier> identifier;
        std::unique_ptr<Expr> initializer;
    };
}
