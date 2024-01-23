#include <memory>
#include <sstream>
#include "ast.hpp"
#include "lexer.hpp"

namespace slim::ast
{
    Node::Node(Token token) : token(token) { }

    BinaryExpr::BinaryExpr(
        Token token,
        std::unique_ptr<Expr> left,
        std::unique_ptr<Expr> right
    ) : Expr(token), left(std::move(left)), right(std::move(right)) { }

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

    UnaryExpr::UnaryExpr(
        Token token,
        std::unique_ptr<Expr> operand
    ) : Expr(token), operand(std::move(operand)) { }

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

    std::string Identifier::Debug()
    {
        std::stringstream ss;
        ss << "id{" << token.ToString() << "}";
        return ss.str();
    }

    std::string NumericLiteral::Debug()
    {
        std::stringstream ss;
        ss << "i{" << token.ToString() << "}";
        return ss.str();
    }

    std::string BooleanLiteral::Debug()
    {
        std::stringstream ss;
        ss << "b{" << token.ToString() << "}";
        return ss.str();
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

    ExprStat::ExprStat(
        Token token,
        std::unique_ptr<Expr> expr
    )
        : Statement(token)
        , expr(std::move(expr))
    { }

    Tag::Tag(Token token, std::unique_ptr<StringLiteral> meta)
        : Node(token)
        , meta(std::move(meta))
    { }

    PropertyDecl::PropertyDecl(
        Token token,
        std::vector<std::unique_ptr<Tag>> tags,
        std::unique_ptr<DeclStat> decl
    )
        : Node(token)
        , tags(std::move(tags))
        , decl(std::move(decl))
    { }

    SharedDecl::SharedDecl(
        Token token,
        std::unique_ptr<DataType> type,
        std::unique_ptr<Identifier> identifier
    )
        : Node(token)
        , type(std::move(type))
        , identifier(std::move(identifier))
    { }

    FeatureBlock::FeatureBlock(
        Token token,
        std::unique_ptr<Identifier> identifier,
        std::vector<std::unique_ptr<PropertyDecl>> decls
    )
        : Node(token)
        , identifier(std::move(identifier))
        , decls(std::move(decls))
    { }
}
