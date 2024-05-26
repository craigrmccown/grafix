#pragma once

#include <memory>
#include <string>
#include <vector>
#include "lexer.hpp"
#include "operators.hpp"
#include "types.hpp"

namespace slim::ast
{
    // Forward declarations to break dependency cycle
    class Visitor;
    class Traverser;

    // Base for all AST node types. Nodes should be considered immutable, with
    // const, public members. Pointers to other AST nodes are non-const so they
    // can still be moved, but should also not be mutated. Metadata associated
    // with each node should be maintained by post-processing steps rather than
    // mutable annotations on nodes themselves. For this purpose, each node
    // holds an ordinal that can be used to uniquely identify it.
    struct Node
    {
        Node(Token token);

        // Implements the visitor pattern. Each implementing class should
        // optionally call its corresponding visitor method.
        virtual void Dispatch(Visitor &visitor) const = 0;

        // Performs a depth-first pre and post order traversal
        virtual void Traverse(Traverser &traverser) const;

        // Ensure destructor of subclass is called when deleted
        virtual ~Node() = default;

        // Can be used as a unique identifier for each node
        const uint32_t ordinal;
        const Token token;

        private:
        inline static uint32_t counter;
    };

    struct Program : public Node
    {
        Program(Token token, std::vector<std::unique_ptr<Node>> children);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::vector<std::unique_ptr<Node>> children;
    };

    struct Expr : public Node
    {
        using Node::Node;

        // Returns a string representation of the expression for debugging
        // and testing
        virtual std::string Debug() const = 0;

        // Ensure destructor of subclass is called when deleted
        virtual ~Expr() = default;
    };

    struct BinaryExpr : public Expr
    {
        BinaryExpr(
            Token token,
            operators::Operator op,
            std::unique_ptr<Expr> left,
            std::unique_ptr<Expr> right
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() const override;

        const operators::Operator op;
        std::unique_ptr<Expr> left, right;
    };

    struct UnaryExpr : public Expr
    {
        UnaryExpr(
            Token token,
            operators::Operator op,
            std::unique_ptr<Expr> operand
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() const override;

        const operators::Operator op;
        std::unique_ptr<Expr> operand;
    };

    struct VariableReference : public Expr
    {
        VariableReference(Token token, std::string name);

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() const override;

        const std::string name;
    };

    struct IntLiteral : public Expr
    {
        IntLiteral(Token token, int32_t value);

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() const override;

        const int32_t value;
    };

    struct FloatLiteral : public Expr
    {
        FloatLiteral(Token token, std::string value);

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() const override;

        // We never need to access the value of a float, so store as a string to
        // avoid any possible floating point precision loss
        const std::string value;
    };

    struct BooleanLiteral : public Expr
    {
        BooleanLiteral(Token token, bool value);

        void Dispatch(Visitor &visitor) const override;
        std::string Debug() const override;

        const bool value;
    };

    // As expressions can only produce numerical values, string literals do not
    // inherit from Expr. However, we still include a string literal type for
    // use outside of expressions, e.g. property tag metadata
    struct StringLiteral : public Node
    {
        using Node::Node;

        void Dispatch(Visitor &visitor) const override;
    };

    struct FieldAccess : public Expr
    {
        FieldAccess(
            Token token,
            std::unique_ptr<Expr> accessed,
            std::string field
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() const override;

        const std::string field;
        std::unique_ptr<Expr> accessed;
    };

    struct FunctionCall : public Expr
    {
        FunctionCall(
            Token token,
            std::unique_ptr<Expr> fn,
            std::vector<std::unique_ptr<Expr>> args
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;
        std::string Debug() const override;

        const std::vector<std::unique_ptr<Expr>> args;
        std::unique_ptr<Expr> fn;
    };

    struct ExprStat : public Node
    {
        ExprStat(Token token, std::unique_ptr<Expr> expr);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        std::unique_ptr<Expr> expr;
    };

    struct ReturnStat : public ExprStat
    {
        using ExprStat::ExprStat;

        void Dispatch(Visitor &visitor) const override;
    };

    struct DeclStat : public Node
    {
        DeclStat(
            Token token,
            std::string type,
            std::string name,
            std::unique_ptr<Expr> initializer = nullptr
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::string type, name;
        std::unique_ptr<Expr> initializer;
    };

    struct Tag : public Node
    {
        Tag(Token token, std::unique_ptr<StringLiteral> meta = nullptr);

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        std::unique_ptr<StringLiteral> meta;
    };

    struct PropertyDecl : public DeclStat
    {
        PropertyDecl(
            Token token,
            std::vector<std::unique_ptr<Tag>> tags,
            std::string type,
            std::string name,
            std::unique_ptr<Expr> initializer = nullptr
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::vector<std::unique_ptr<Tag>> tags;
    };

    struct SharedDecl : public DeclStat
    {
        using DeclStat::DeclStat;

        void Dispatch(Visitor &visitor) const override;
    };

    struct FeatureBlock : public Node
    {
        FeatureBlock(
            Token token,
            std::string name,
            std::vector<std::unique_ptr<PropertyDecl>> decls
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::string name;
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
            std::vector<std::unique_ptr<ast::Node>> stats
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const ShaderType type;
        const std::vector<std::unique_ptr<ast::Node>> stats;
    };

    struct RequireBlock : public Node
    {
        RequireBlock(
            Token token,
            std::string feature,
            std::vector<std::unique_ptr<ast::Node>> stats
        );

        void Dispatch(Visitor &visitor) const override;
        void Traverse(Traverser &traverser) const override;

        const std::string feature;
        const std::vector<std::unique_ptr<ast::Node>> stats;
    };

    class Visitor
    {
        public:
        virtual void VisitProgram(const Program &node) = 0;
        virtual void VisitBinaryExpr(const BinaryExpr &node) = 0;
        virtual void VisitUnaryExpr(const UnaryExpr &node) = 0;
        virtual void VisitVariableReference(const VariableReference &node) = 0;
        virtual void VisitIntLiteral(const IntLiteral &node) = 0;
        virtual void VisitFloatLiteral(const FloatLiteral &node) = 0;
        virtual void VisitBooleanLiteral(const BooleanLiteral &node) = 0;
        virtual void VisitStringLiteral(const StringLiteral &node) = 0;
        virtual void VisitFieldAccess(const FieldAccess &node) = 0;
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

    // A Visitor implementation that does nothing. Other Visitor implementations
    // can inherit from this class and override specific methods to selectively
    // process AST nodes. Provided for convenience.
    class NoopVisitor : public Visitor
    {
        public:
        virtual void VisitProgram(const Program &node) override;
        virtual void VisitBinaryExpr(const ast::BinaryExpr &node) override;
        virtual void VisitUnaryExpr(const ast::UnaryExpr &node) override;
        virtual void VisitVariableReference(const ast::VariableReference &node) override;
        virtual void VisitIntLiteral(const ast::IntLiteral &node) override;
        virtual void VisitFloatLiteral(const ast::FloatLiteral &node) override;
        virtual void VisitBooleanLiteral(const ast::BooleanLiteral &node) override;
        virtual void VisitStringLiteral(const ast::StringLiteral &node) override;
        virtual void VisitFieldAccess(const ast::FieldAccess &node) override;
        virtual void VisitFunctionCall(const ast::FunctionCall &node) override;
        virtual void VisitExprStat(const ast::ExprStat &node) override;
        virtual void VisitReturnStat(const ast::ReturnStat &node) override;
        virtual void VisitDeclStat(const ast::DeclStat &node) override;
        virtual void VisitTag(const ast::Tag &node) override;
        virtual void VisitPropertyDecl(const ast::PropertyDecl &node) override;
        virtual void VisitSharedDecl(const ast::SharedDecl &node) override;
        virtual void VisitFeatureBlock(const ast::FeatureBlock &node) override;
        virtual void VisitShaderBlock(const ast::ShaderBlock &node) override;
        virtual void VisitRequireBlock(const ast::RequireBlock &node) override;
    };

    class Traverser
    {
        public:
        virtual void Pre(const Node &node) = 0;
        virtual void Post(const Node &node) = 0;
    };
}
