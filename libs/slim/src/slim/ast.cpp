#include <memory>
#include <string>
#include "ast.hpp"
#include "lexer.hpp"
#include "operators.hpp"

namespace slim::ast
{
    Node::Node(Token token) : token(token), ordinal(Node::counter++) { }

    // Implement leaf node traversal by default. Non-leaf classes should
    // override
    void Node::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        traverser.Post(*this);
    }

    BinaryExpr::BinaryExpr(
        Token token,
        operators::Operator op,
        std::unique_ptr<Expr> left,
        std::unique_ptr<Expr> right
    )
        : Expr(token)
        , op(op)
        , left(std::move(left))
        , right(std::move(right))
    { }

    void BinaryExpr::Dispatch(Visitor &visitor) const
    {
        visitor.VisitBinaryExpr(*this);
    }

    void BinaryExpr::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        left->Traverse(traverser);
        right->Traverse(traverser);
        traverser.Post(*this);
    }

    std::string BinaryExpr::Debug() const
    {
        return
            "(" + operators::toString(op) + " " +
            left->Debug() + " " + right->Debug() + ")";
    }

    UnaryExpr::UnaryExpr(
        Token token,
        operators::Operator op,
        std::unique_ptr<Expr> operand
    )
        : Expr(token)
        , op(op)
        , operand(std::move(operand))
    { }

    void UnaryExpr::Dispatch(Visitor &visitor) const
    {
        visitor.VisitUnaryExpr(*this);
    }

    void UnaryExpr::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        operand->Traverse(traverser);
        traverser.Post(*this);
    }

    std::string UnaryExpr::Debug() const
    {
        return
            "(" + operators::toString(op) + " " +
            operand->Debug() + ")";
    }

    VariableReference::VariableReference(Token token, std::string name)
        : Expr(token)
        , name(name)
        { }

    void VariableReference::Dispatch(Visitor &visitor) const
    {
        visitor.VisitVariableReference(*this);
    }

    std::string VariableReference::Debug() const
    {
        return "var{" + name + "}";
    }

    IntLiteral::IntLiteral(Token token, int32_t value)
        : Expr(token)
        , value(value)
        { }

    void IntLiteral::Dispatch(Visitor &visitor) const
    {
        visitor.VisitIntLiteral(*this);
    }

    std::string IntLiteral::Debug() const
    {
        return "i{" + std::to_string(value) + "}";
    }

    FloatLiteral::FloatLiteral(Token token, std::string value)
        : Expr(token)
        , value(value)
        { }

    std::string FloatLiteral::Debug() const
    {
        return "f{" + value + "}";
    }

    void FloatLiteral::Dispatch(Visitor &visitor) const
    {
        visitor.VisitFloatLiteral(*this);
    }

    BooleanLiteral::BooleanLiteral(Token token, bool value)
        : Expr(token)
        , value(value)
        { }

    void BooleanLiteral::Dispatch(Visitor &visitor) const
    {
        visitor.VisitBooleanLiteral(*this);
    }

    std::string BooleanLiteral::Debug() const
    {
        std::string s = value ? "true" : "false";
        return "b{" + s + "}";
    }

    void StringLiteral::Dispatch(Visitor &visitor) const
    {
        visitor.VisitStringLiteral(*this);
    }

    FieldAccess::FieldAccess(
        Token token,
        std::unique_ptr<Expr> accessed,
        std::string field
    )
        : Expr(token)
        , accessed(std::move(accessed))
        , field(field)
    { }

    void FieldAccess::Dispatch(Visitor &visitor) const
    {
        visitor.VisitFieldAccess(*this);
    }

    void FieldAccess::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        accessed->Traverse(traverser);
        traverser.Post(*this);
    }

    std::string FieldAccess::Debug() const
    {
        return "(. " + accessed->Debug() + " " + field + ")";
    }

    FunctionCall::FunctionCall(
        Token token,
        std::unique_ptr<Expr> fn,
        std::vector<std::unique_ptr<Expr>> args
    )
        : Expr(token)
        , fn(std::move(fn))
        , args(std::move(args))
    { }

    void FunctionCall::Dispatch(Visitor &visitor) const
    {
        visitor.VisitFunctionCall(*this);
    }

    void FunctionCall::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        fn->Traverse(traverser);

        for (const std::unique_ptr<Expr> &arg : args)
        {
            arg->Traverse(traverser);
        }

        traverser.Post(*this);
    }

    std::string FunctionCall::Debug() const
    {
        std::string s = "(" + fn->Debug();
        for (const std::unique_ptr<Expr>& arg : args)
        {
            s += " " + arg->Debug();
        }
        return s + ")";
    }

    DeclStat::DeclStat(
        Token token,
        std::string type,
        std::string name,
        std::unique_ptr<Expr> initializer
    )
        : Node(token)
        , type(std::move(type))
        , name(std::move(name))
        , initializer(std::move(initializer))
    { }

    void DeclStat::Dispatch(Visitor &visitor) const
    {
        visitor.VisitDeclStat(*this);
    }

    void DeclStat::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        initializer->Traverse(traverser);
        traverser.Post(*this);
    }

    ExprStat::ExprStat(
        Token token,
        std::unique_ptr<Expr> expr
    )
        : Node(token)
        , expr(std::move(expr))
    { }

    void ExprStat::Dispatch(Visitor &visitor) const
    {
        visitor.VisitExprStat(*this);
    }

    void ExprStat::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        expr->Traverse(traverser);
        traverser.Post(*this);
    }

    void ReturnStat::Dispatch(Visitor &visitor) const
    {
        visitor.VisitReturnStat(*this);
    }

    Tag::Tag(Token token, std::unique_ptr<StringLiteral> meta)
        : Node(token)
        , meta(std::move(meta))
    { }

    void Tag::Dispatch(Visitor &visitor) const
    {
        visitor.VisitTag(*this);
    }

    void Tag::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        meta->Traverse(traverser);
        traverser.Post(*this);
    }

    PropertyDecl::PropertyDecl(
        Token token,
        std::vector<std::unique_ptr<Tag>> tags,
        std::string type,
        std::string name,
        std::unique_ptr<Expr> initializer
    )
        : DeclStat(token, std::move(type), std::move(name), std::move(initializer))
        , tags(std::move(tags))
    { }

    void PropertyDecl::Dispatch(Visitor &visitor) const
    {
        visitor.VisitPropertyDecl(*this);
    }

    void PropertyDecl::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);

        for (const std::unique_ptr<Tag> &tag : tags)
        {
            tag->Traverse(traverser);
        }

        DeclStat::Traverse(traverser);
        traverser.Post(*this);
    }

    void SharedDecl::Dispatch(Visitor &visitor) const
    {
        visitor.VisitSharedDecl(*this);
    }

    FeatureBlock::FeatureBlock(
        Token token,
        std::string name,
        std::vector<std::unique_ptr<PropertyDecl>> decls
    )
        : Node(token)
        , name(name)
        , decls(std::move(decls))
    { }

    void FeatureBlock::Dispatch(Visitor &visitor) const
    {
        visitor.VisitFeatureBlock(*this);
    }

    void FeatureBlock::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);

        for (const std::unique_ptr<PropertyDecl> &decl : decls)
        {
            decl->Traverse(traverser);
        }

        traverser.Post(*this);
    }

    ShaderBlock::ShaderBlock(
        Token token,
        ShaderType type,
        std::vector<std::unique_ptr<ast::Node>> stats
    )
        : Node(token)
        , type(type)
        , stats(std::move(stats))
    { }

    void ShaderBlock::Dispatch(Visitor &visitor) const
    {
        visitor.VisitShaderBlock(*this);
    }

    void ShaderBlock::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);

        for (const std::unique_ptr<Node> &stat : stats)
        {
            stat->Traverse(traverser);
        }

        traverser.Post(*this);
    }

    RequireBlock::RequireBlock(
        Token token,
        std::string feature,
        std::vector<std::unique_ptr<ast::Node>> stats
    )
        : Node(token)
        , feature(feature)
        , stats(std::move(stats))
    { }

    void RequireBlock::Dispatch(Visitor &visitor) const
    {
        visitor.VisitRequireBlock(*this);
    }

    void RequireBlock::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);

        for (const std::unique_ptr<Node> &stat : stats)
        {
            stat->Traverse(traverser);
        }

        traverser.Post(*this);
    }

    void NoopVisitor::VisitBinaryExpr(const ast::BinaryExpr &node) { }
    void NoopVisitor::VisitUnaryExpr(const ast::UnaryExpr &node) { }
    void NoopVisitor::VisitVariableReference(const ast::VariableReference &node) { }
    void NoopVisitor::VisitIntLiteral(const ast::IntLiteral &node) { }
    void NoopVisitor::VisitFloatLiteral(const ast::FloatLiteral &node) { }
    void NoopVisitor::VisitBooleanLiteral(const ast::BooleanLiteral &node) { }
    void NoopVisitor::VisitStringLiteral(const ast::StringLiteral &node) { }
    void NoopVisitor::VisitFieldAccess(const ast::FieldAccess &node) { }
    void NoopVisitor::VisitFunctionCall(const ast::FunctionCall &node) { }
    void NoopVisitor::VisitExprStat(const ast::ExprStat &node) { }
    void NoopVisitor::VisitReturnStat(const ast::ReturnStat &node) { }
    void NoopVisitor::VisitDeclStat(const ast::DeclStat &node) { }
    void NoopVisitor::VisitTag(const ast::Tag &node) { }
    void NoopVisitor::VisitPropertyDecl(const ast::PropertyDecl &node) { }
    void NoopVisitor::VisitSharedDecl(const ast::SharedDecl &node) { }
    void NoopVisitor::VisitFeatureBlock(const ast::FeatureBlock &node) { }
    void NoopVisitor::VisitShaderBlock(const ast::ShaderBlock &node) { }
    void NoopVisitor::VisitRequireBlock(const ast::RequireBlock &node) { }
}
