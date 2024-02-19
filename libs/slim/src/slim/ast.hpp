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

    struct Node
    {
        Node(Token token);

        // Implements the visitor pattern. Each implementing class should
        // optionally call its corresponding visitor method.
        virtual void Dispatch(Visitor &visitor) const = 0;

        // Performs a depth-first pre and post order traversal
        virtual void Traverse(Traverser &traverser) const;

        const uint32_t ordinal;
        const Token token;

        private:
        static uint32_t counter;
    };

    struct Expr : public Node
    {
        using Node::Node;

        virtual std::string Debug() = 0;

        // Ensure destructor of subclass is called when deleted
        virtual ~Expr() = default;
    };

    struct BinaryExpr : public Expr
    {
        BinaryExpr(Token token, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);

        void Traverse(Traverser &traverser) const override;
        std::string Debug() override;

        const std::unique_ptr<Expr> left, right;
    };

    struct AssignmentExpr : public BinaryExpr
    {
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
    };

    struct BooleanExpr : public BinaryExpr
    {
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
    };

    struct ComparisonExpr : public BinaryExpr
    {
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
    };

    struct ArithmeticExpr : public BinaryExpr
    {
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
    };

    struct IndexAccess : public BinaryExpr
    {
        using BinaryExpr::BinaryExpr;

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() override;
    };

    struct UnaryExpr : public Expr
    {
        UnaryExpr(Token token, std::unique_ptr<Expr> operand);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() override;

        const std::unique_ptr<Expr> operand;
    };

    struct Identifier : public Expr
    {
        using Expr::Expr;

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() override;
    };

    struct NumericLiteral : public Expr
    {
        using Expr::Expr;

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() override;
    };

    struct BooleanLiteral : public Expr
    {
        using Expr::Expr;

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() override;
    };

    // As expressions can only produce numerical values, string literals do not
    // inherit from Expr. However, we still include a string literal type for
    // use outside of expressions, e.g. property tag metadata
    struct StringLiteral : public Node
    {
        using Node::Node;

        void Dispatch(Visitor &visitor) const override;
    };

    struct DataType : public Node
    {
        using Node::Node;

        void Dispatch(Visitor &visitor) const override;
    };

    struct PropertyAccess : public Expr
    {
        PropertyAccess(Token token, std::unique_ptr<Expr> accessed, std::unique_ptr<Identifier> property);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() override;

        const std::unique_ptr<Expr> accessed;
        const std::unique_ptr<Identifier> property;
    };

    struct FunctionCall : public Expr
    {
        FunctionCall(Token token, std::unique_ptr<Expr> fn, std::vector<std::unique_ptr<Expr>> args);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() override;

        const std::unique_ptr<Expr> fn;
        const std::vector<std::unique_ptr<Expr>> args;
    };

    struct Statement : public Node
    {
        using Node::Node;

        // Ensure destructor of subclass is called when deleted
        virtual ~Statement() = default;
    };

    struct ExprStat : public Statement
    {
        ExprStat(Token token, std::unique_ptr<Expr> expr);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::unique_ptr<Expr> expr;
    };

    struct ReturnStat : public ExprStat
    {
        using ExprStat::ExprStat;

        void Dispatch(Visitor &visitor) const override;
    };

    struct DeclStat : public Statement
    {
        DeclStat(
            Token token,
            std::unique_ptr<DataType> type,
            std::unique_ptr<Identifier> identifier,
            std::unique_ptr<Expr> initializer = nullptr
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::unique_ptr<DataType> type;
        const std::unique_ptr<Identifier> identifier;
        const std::unique_ptr<Expr> initializer;
    };

    struct Tag : public Node
    {
        Tag(Token token, std::unique_ptr<StringLiteral> meta = nullptr);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::unique_ptr<StringLiteral> meta;
    };

    struct PropertyDecl : public Node
    {
        PropertyDecl(
            Token token,
            std::vector<std::unique_ptr<Tag>> tags,
            std::unique_ptr<DeclStat> decl
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::vector<std::unique_ptr<Tag>> tags;
        const std::unique_ptr<DeclStat> decl;
    };

    struct SharedDecl : public Node
    {
        SharedDecl(
            Token token,
            std::unique_ptr<DataType> type,
            std::unique_ptr<Identifier> identifier
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::unique_ptr<DataType> type;
        const std::unique_ptr<Identifier> identifier;
    };

    struct FeatureBlock : public Node
    {
        FeatureBlock(
            Token token,
            std::unique_ptr<Identifier> identifier,
            std::vector<std::unique_ptr<PropertyDecl>> decls
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::unique_ptr<Identifier> identifier;
        const std::vector<std::unique_ptr<PropertyDecl>> decls;
    };

    struct ShaderBlock : public Node
    {
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

        const ShaderType type;
        const std::vector<std::unique_ptr<ast::Statement>> stats;
    };

    struct RequireBlock : public Statement
    {
        RequireBlock(
            Token token,
            std::unique_ptr<ast::Identifier> identifier,
            std::vector<std::unique_ptr<ast::Statement>> stats
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::unique_ptr<ast::Identifier> identifier;
        const std::vector<std::unique_ptr<ast::Statement>> stats;
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
