#pragma once

#include <memory>
#include <string>
#include <vector>
#include "lexer.hpp"

namespace slim::ast
{
    // Forward declarations to break dependency cycle
    class Visitor;
    class Traverser;

    class Node
    {
        public:
        Node(Token token);

        // Implements the visitor pattern. Each implementing class should
        // optionally call its corresponding visitor method.
        virtual void Dispatch(Visitor &visitor) const = 0;

        // Performs a depth-first pre and post order traversal
        virtual void Traverse(Traverser &traverser) const;

        protected:
        Token token;
    };

    class Expr : public Node
    {
        public:
        using Node::Node;

        virtual std::string Debug() = 0;

        // Ensure destructor of subclass is called when deleted
        virtual ~Expr() = default;
    };

    class BinaryExpr : public Expr
    {
        public:
        BinaryExpr(Token token, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);

        void Traverse(Traverser &traverser) const override;
        std::string Debug() override;

        protected:
        std::unique_ptr<Expr> left, right;
    };

    class AssignmentExpr : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
    };

    class BooleanExpr : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
    };

    class ComparisonExpr : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
    };

    class ArithmeticExpr : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
    };

    class IndexAccess : public BinaryExpr
    {
        public:
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() override;
    };

    class UnaryExpr : public Expr
    {
        public:
        UnaryExpr(Token token, std::unique_ptr<Expr> operand);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() override;

        private:
        std::unique_ptr<Expr> operand;
    };

    class Identifier : public Expr
    {
        public:
        using Expr::Expr;

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() override;
    };

    class NumericLiteral : public Expr
    {
        public:
        using Expr::Expr;

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() override;
    };

    class BooleanLiteral : public Expr
    {
        public:
        using Expr::Expr;

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() override;
    };

    // As expressions can only produce numerical values, string literals do not
    // inherit from Expr. However, we still include a string literal type for
    // use outside of expressions, e.g. property tag metadata
    class StringLiteral : public Node
    {
        public:
        using Node::Node;

        void Dispatch(Visitor &visitor) const override;
    };

    class DataType : public Node
    {
        public:
        using Node::Node;

        void Dispatch(Visitor &visitor) const override;
    };

    class PropertyAccess : public Expr
    {
        public:
        PropertyAccess(Token token, std::unique_ptr<Expr> accessed, std::unique_ptr<Identifier> property);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() override;

        private:
        std::unique_ptr<Expr> accessed;
        std::unique_ptr<Identifier> property;
    };

    class FunctionCall : public Expr
    {
        public:
        FunctionCall(Token token, std::unique_ptr<Expr> fn, std::vector<std::unique_ptr<Expr>> args);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() override;

        private:
        std::unique_ptr<Expr> fn;
        std::vector<std::unique_ptr<Expr>> args;
    };

    class Statement : public Node
    {
        public:
        using Node::Node;

        // Ensure destructor of subclass is called when deleted
        virtual ~Statement() = default;
    };

    class ExprStat : public Statement
    {
        public:
        ExprStat(Token token, std::unique_ptr<Expr> expr);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        private:
        std::unique_ptr<Expr> expr;
    };

    class ReturnStat : public ExprStat
    {
        using ExprStat::ExprStat;

        void Dispatch(Visitor &visitor) const override;
    };

    class DeclStat : public Statement
    {
        public:
        DeclStat(
            Token token,
            std::unique_ptr<DataType> type,
            std::unique_ptr<Identifier> identifier,
            std::unique_ptr<Expr> initializer = nullptr
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        private:
        std::unique_ptr<DataType> type;
        std::unique_ptr<Identifier> identifier;
        std::unique_ptr<Expr> initializer;
    };

    class Tag : public Node
    {
        public:
        Tag(Token token, std::unique_ptr<StringLiteral> meta = nullptr);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        private:
        std::unique_ptr<StringLiteral> meta;
    };

    class PropertyDecl : public Node
    {
        public:
        PropertyDecl(
            Token token,
            std::vector<std::unique_ptr<Tag>> tags,
            std::unique_ptr<DeclStat> decl
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        private:
        std::vector<std::unique_ptr<Tag>> tags;
        std::unique_ptr<DeclStat> decl;
    };

    class SharedDecl : public Node
    {
        public:
        SharedDecl(
            Token token,
            std::unique_ptr<DataType> type,
            std::unique_ptr<Identifier> identifier
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        private:
        std::unique_ptr<DataType> type;
        std::unique_ptr<Identifier> identifier;
    };

    class FeatureBlock : public Node
    {
        public:
        FeatureBlock(
            Token token,
            std::unique_ptr<Identifier> identifier,
            std::vector<std::unique_ptr<PropertyDecl>> decls
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        private:
        std::unique_ptr<Identifier> identifier;
        std::vector<std::unique_ptr<PropertyDecl>> decls;
    };

    class ShaderBlock : public Node
    {
        public:
        enum ShaderType
        {
            Vertex,
            Fragment
        };

        ShaderBlock(
            Token token,
            ShaderType type,
            std::vector<std::unique_ptr<ast::Statement>> stats
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        private:
        ShaderType type;
        std::vector<std::unique_ptr<ast::Statement>> stats;
    };

    class RequireBlock : public Statement
    {
        public:
        RequireBlock(
            Token token,
            std::unique_ptr<ast::Identifier> identifier,
            std::vector<std::unique_ptr<ast::Statement>> stats
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        private:
        std::unique_ptr<ast::Identifier> identifier;
        std::vector<std::unique_ptr<ast::Statement>> stats;
    };

    class Visitor
    {
        public:
        virtual void VisitAssignmentExpr(const AssignmentExpr &node) = 0;
        virtual void VisitBooleanExpr(const BooleanExpr &node) = 0;
        virtual void VisitComparisonExpr(const ComparisonExpr &node) = 0;
        virtual void VisitArithmeticExpr(const ArithmeticExpr &node) = 0;
        virtual void VisitIndexAccess(const IndexAccess &node) = 0;
        virtual void VisitUnaryExpr(const UnaryExpr &node) = 0;
        virtual void VisitIdentifier(const Identifier &node) = 0;
        virtual void VisitNumericLiteral(const NumericLiteral &node) = 0;
        virtual void VisitBooleanLiteral(const BooleanLiteral &node) = 0;
        virtual void VisitStringLiteral(const StringLiteral &node) = 0;
        virtual void VisitDataType(const DataType &node) = 0;
        virtual void VisitPropertyAccess(const PropertyAccess &node) = 0;
        virtual void VisitFunctionCall(const FunctionCall &node) = 0;
        virtual void VisitExprStat(const ExprStat &node) = 0;
        virtual void VisitReturnStat(const ReturnStat &node) = 0;
        virtual void VisitDeclStat(const DeclStat &node) = 0;
        virtual void VisitTag(const Tag &node) = 0;
        virtual void VisitPropertyDecl(const PropertyDecl &node) = 0;
        virtual void VisitSharedDecl(const SharedDecl &node) = 0;
        virtual void VisitFeatureBlock(const FeatureBlock &node) = 0;
        virtual void VisitShaderBlock(const ShaderBlock &node) = 0;
        virtual void VisitRequireBlock(const RequireBlock &node) = 0;
    };

    class Traverser
    {
        public:
        virtual void Pre(const Node &node) = 0;
        virtual void Post(const Node &node) = 0;
    };
}
