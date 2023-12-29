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

    std::string StringLiteral::Debug()
    {
        // Surrounding double quotes are implicitly included
        return token.ToString();
    }

    std::string DataType::Debug()
    {
        return token.ToString();
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

    Tag::Tag(Token token, std::unique_ptr<StringLiteral> meta)
        : Node(token)
        , meta(std::move(meta))
    { }

    // Example: (#mytag "meta")
    std::string Tag::Debug()
    {
        std::stringstream ss;
        ss << "(" << token.ToString();
        if (meta)
        {
            ss << " " << meta->Debug();
        }
        ss << ")";
        return ss.str();
    }

    PropertyDecl::PropertyDecl(
        Token token,
        std::vector<std::unique_ptr<Tag>> tags,
        std::unique_ptr<DataType> type,
        std::unique_ptr<Identifier> identifier,
        std::unique_ptr<Expr> initializer
    )
        : Statement(token)
        , tags(std::move(tags))
        , type(std::move(type))
        , identifier(std::move(identifier))
        , initializer(std::move(initializer))
    { }

    // e.g. (property (tags (#mytag "meta") (#othertag)) vec3 id{myprop} (id{vec3} i{0} i{0} i{0}))
    std::string PropertyDecl::Debug()
    {
        std::stringstream ss;
        ss << "(property (tags";

        for (const std::unique_ptr<ast::Tag> &tag : tags)
        {
            ss << " " << tag->Debug();
        }

        ss << ") " << type->Debug() << " " << identifier->Debug();

        if (initializer)
        {
            ss << " " << initializer->Debug();
        }

        ss << ")";
        return ss.str();
    }
}
