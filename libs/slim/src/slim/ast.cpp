#include <memory>
#include <sstream>
#include "ast.hpp"
#include "lexer.hpp"

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
        std::unique_ptr<Expr> left,
        std::unique_ptr<Expr> right
    ) : Expr(token), left(std::move(left)), right(std::move(right)) { }

    void BinaryExpr::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        left->Traverse(traverser);
        right->Traverse(traverser);
        traverser.Post(*this);
    }

    std::string BinaryExpr::Debug()
    {
        std::stringstream ss;
        ss
            << "("
            << token.ToString()
            << " "
            << left->Debug()
            << " "
            << right->Debug()
            << ")";
        return ss.str();
    }

    void AssignmentExpr::Dispatch(Visitor &visitor) const
    {
        visitor.VisitAssignmentExpr(*this);
    }

    void BooleanExpr::Dispatch(Visitor &visitor) const
    {
        visitor.VisitBooleanExpr(*this);
    }

    void ComparisonExpr::Dispatch(Visitor &visitor) const
    {
        visitor.VisitComparisonExpr(*this);
    }

    void ArithmeticExpr::Dispatch(Visitor &visitor) const
    {
        visitor.VisitArithmeticExpr(*this);
    }

    std::string IndexAccess::Debug()
    {
        std::stringstream ss;
        ss
            << "([] "
            << left->Debug()
            << " "
            << right->Debug()
            << ")";
        return ss.str();
    }

    void IndexAccess::Dispatch(Visitor &visitor) const
    {
        visitor.VisitIndexAccess(*this);
    }

    UnaryExpr::UnaryExpr(
        Token token,
        std::unique_ptr<Expr> operand
    ) : Expr(token), operand(std::move(operand)) { }

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

    std::string UnaryExpr::Debug()
    {
       std::stringstream ss;
        ss
            << "("
            << token.ToString()
            << " "
            << operand->Debug()
            << ")";
        return ss.str();
    }

    void Identifier::Dispatch(Visitor &visitor) const
    {
        visitor.VisitIdentifier(*this);
    }

    std::string Identifier::Debug()
    {
        std::stringstream ss;
        ss << "id{" << token.ToString() << "}";
        return ss.str();
    }

    void NumericLiteral::Dispatch(Visitor &visitor) const
    {
        visitor.VisitNumericLiteral(*this);
    }

    std::string NumericLiteral::Debug()
    {
        std::stringstream ss;
        ss << "i{" << token.ToString() << "}";
        return ss.str();
    }

    void BooleanLiteral::Dispatch(Visitor &visitor) const
    {
        visitor.VisitBooleanLiteral(*this);
    }

    std::string BooleanLiteral::Debug()
    {
        std::stringstream ss;
        ss << "b{" << token.ToString() << "}";
        return ss.str();
    }

    void StringLiteral::Dispatch(Visitor &visitor) const
    {
        visitor.VisitStringLiteral(*this);
    }

    void DataType::Dispatch(Visitor &visitor) const
    {
        visitor.VisitDataType(*this);
    }

    PropertyAccess::PropertyAccess(
        Token token,
        std::unique_ptr<Expr> accessed,
        std::unique_ptr<Identifier> property
    )
        : Expr(token)
        , accessed(std::move(accessed))
        , property(std::move(property))
    { }

    void PropertyAccess::Dispatch(Visitor &visitor) const
    {
        visitor.VisitPropertyAccess(*this);
    }

    void PropertyAccess::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        accessed->Traverse(traverser);
        property->Traverse(traverser);
        traverser.Post(*this);
    }

    std::string PropertyAccess::Debug()
    {
        std::stringstream ss;
        ss
            << "(. "
            << accessed->Debug()
            << " "
            << property->Debug()
            << ")";
        return ss.str();
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

    std::string FunctionCall::Debug()
    {
        std::stringstream ss;
        ss << "(" << fn->Debug();
        for (const std::unique_ptr<Expr>& arg : args)
        {
            ss << " " << arg->Debug();
        }
        ss << ")";
        return ss.str();
    }

    DeclStat::DeclStat(
        Token token,
        std::unique_ptr<DataType> type,
        std::unique_ptr<Identifier> identifier,
        std::unique_ptr<Expr> initializer
    )
        : Statement(token)
        , type(std::move(type))
        , identifier(std::move(identifier))
        , initializer(std::move(initializer))
    { }

    void DeclStat::Dispatch(Visitor &visitor) const
    {
        visitor.VisitDeclStat(*this);
    }

    void DeclStat::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        type->Traverse(traverser);
        identifier->Traverse(traverser);
        initializer->Traverse(traverser);
        traverser.Post(*this);
    }

    ExprStat::ExprStat(
        Token token,
        std::unique_ptr<Expr> expr
    )
        : Statement(token)
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
        std::unique_ptr<DeclStat> decl
    )
        : Node(token)
        , tags(std::move(tags))
        , decl(std::move(decl))
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

        decl->Traverse(traverser);
        traverser.Post(*this);
    }

    SharedDecl::SharedDecl(
        Token token,
        std::unique_ptr<DataType> type,
        std::unique_ptr<Identifier> identifier
    )
        : Node(token)
        , type(std::move(type))
        , identifier(std::move(identifier))
    { }

    void SharedDecl::Dispatch(Visitor &visitor) const
    {
        visitor.VisitSharedDecl(*this);
    }

    void SharedDecl::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        type->Traverse(traverser);
        identifier->Traverse(traverser);
        traverser.Post(*this);
    }

    FeatureBlock::FeatureBlock(
        Token token,
        std::unique_ptr<Identifier> identifier,
        std::vector<std::unique_ptr<PropertyDecl>> decls
    )
        : Node(token)
        , identifier(std::move(identifier))
        , decls(std::move(decls))
    { }

    void FeatureBlock::Dispatch(Visitor &visitor) const
    {
        visitor.VisitFeatureBlock(*this);
    }

    void FeatureBlock::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        identifier->Traverse(traverser);

        for (const std::unique_ptr<PropertyDecl> &decl : decls)
        {
            decl->Traverse(traverser);
        }

        traverser.Post(*this);
    }

    ShaderBlock::ShaderBlock(
        Token token,
        ShaderType type,
        std::vector<std::unique_ptr<ast::Statement>> stats
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

        for (const std::unique_ptr<Statement> &stat : stats)
        {
            stat->Traverse(traverser);
        }

        traverser.Post(*this);
    }

    RequireBlock::RequireBlock(
        Token token,
        std::unique_ptr<ast::Identifier> identifier,
        std::vector<std::unique_ptr<ast::Statement>> stats
    )
        : Statement(token)
        , identifier(std::move(identifier))
        , stats(std::move(stats))
    { }

    void RequireBlock::Dispatch(Visitor &visitor) const
    {
        visitor.VisitRequireBlock(*this);
    }

    void RequireBlock::Traverse(Traverser &traverser) const
    {
        traverser.Pre(*this);
        identifier->Traverse(traverser);

        for (const std::unique_ptr<Statement> &stat : stats)
        {
            stat->Traverse(traverser);
        }

        traverser.Post(*this);
    }
}
