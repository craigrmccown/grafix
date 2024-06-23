#include <memory>
#include <optional>
#include <string>
#include <variant>
#include "ast.hpp"
#include "operators.hpp"
#include "typecheck.hpp"
#include "types.hpp"

namespace slim::typecheck
{
    static void throwOperatorNotSupported(
        Token token,
        operators::Operator op,
        const types::TypeRef &type
    )
    {
        token.Throw(
            "Cannot perform operation '" + operators::toString(op) +
            "' on value of type '" + types::getTypeName(type) +
            "' because the operator does not support this type"
        );
    }

    static void throwIncompatibleTypes(
        Token token,
        const types::TypeRef &type1,
        const types::TypeRef &type2
    )
    {
        token.Throw(
            "Cannot operate on values of type '" + types::getTypeName(type1) +
            "' and '" + types::getTypeName(type2) +
            "' because their types are incompatible"
        );
    }

    static std::optional<types::TypeRef> resolveMatVecMult(
        const types::TypeRef &tLeft,
        const types::TypeRef &tRight
    )
    {
        if (auto m = std::get_if<std::shared_ptr<types::Matrix>>(&tLeft))
        {
            if (auto v = std::get_if<std::shared_ptr<types::Vector>>(&tRight))
            {
                if ((*m)->size == (*v)->length) return tRight;
            }
        }

        return std::nullopt;
    }

    static types::TypeRef resolveBinaryOperation(
        Token token,
        operators::Operator op,
        const types::TypeRef &tLeft,
        const types::TypeRef &tRight
    )
    {
        if (!operators::supportsLeft(op, tLeft))
        {
            throwOperatorNotSupported(token, op, tLeft);
        }

        if (!operators::supportsRight(op, tRight))
        {
            throwOperatorNotSupported(token, op, tRight);
        }

        // For most operators, type compatibility is decided by equality. No
        // effort is made to implicitly convert types - types must be explicitly
        // converted by the programmer
        switch (op)
        {
            case operators::Or:
            case operators::And:
            case operators::Eq:
            case operators::Neq:
            case operators::Gt:
            case operators::Lt:
            case operators::Ge:
            case operators::Le:
                if (tLeft != tRight)
                {
                    throwIncompatibleTypes(token, tLeft, tRight);
                }

                return types::boolType;
            case operators::Assign:
            case operators::Add:
            case operators::Sub:
            case operators::Div:
            case operators::Mod:
                if (tLeft != tRight)
                {
                    throwIncompatibleTypes(token, tLeft, tRight);
                }

                return tLeft;
            case operators::Mul:
                // Multiplication is overloaded for matrix multiplication, so we
                // handle it separately
                if (tLeft == tRight) return tLeft;

                // Vectors are interpreted as either row or column major when
                // multiplied by matrices, depending on whether they are on the
                // left or right
                if (auto colMajor = resolveMatVecMult(tLeft, tRight)) return *colMajor;
                if (auto rowMajor = resolveMatVecMult(tRight, tLeft)) return *rowMajor;

                throwIncompatibleTypes(token, tLeft, tRight);
            case operators::Index:
                if (auto p = std::get_if<std::shared_ptr<types::Vector>>(&tLeft))
                {
                    return (*p)->GetUnderlyingType();
                }
                else if (auto p = std::get_if<std::shared_ptr<types::Matrix>>(&tLeft))
                {
                    return types::getVectorType(types::Scalar::Float, (*p)->size);
                }
                else
                {
                    // TODO: We should never reach this code, but it's needed
                    // for exhaustiveness. Consider restructuring to avoid doing
                    // this.
                    throw std::runtime_error(
                        "Invariant violation: Type '" + types::getTypeName(tLeft) +
                        "' cannot be indexed"
                    );
                }
            case operators::Not:
                throw std::runtime_error(
                    "Operator '" + operators::toString(op) +
                    "' does not take two operands"
                );
        }
    }

    PreorderVisitor::PreorderVisitor(
        types::SymbolTable &symbols,
        types::TypeRegistry &types
    )
        : symbols(symbols)
        , types(types)
        { }

    PostorderVisitor::PostorderVisitor(
        types::SymbolTable &symbols,
        types::TypeRegistry &types
    )
        : symbols(symbols)
        , types(types)
        { }

    void PostorderVisitor::VisitBinaryExpr(const ast::BinaryExpr &node)
    {
        types::TypeRef tLeft = symbols.CurrentScope().Lookup(node.left->ordinal);
        types::TypeRef tRight = symbols.CurrentScope().Lookup(node.right->ordinal);

        symbols.CurrentScope().Annotate(
            node.ordinal,
            resolveBinaryOperation(node.token, node.op, tLeft, tRight)
        );
    }

    void PostorderVisitor::VisitUnaryExpr(const ast::UnaryExpr &node)
    {
        types::TypeRef tOperand = symbols.CurrentScope().Lookup(node.operand->ordinal);

        if (!operators::supportsLeft(node.op, tOperand))
        {
            throwOperatorNotSupported(node.token, node.op, tOperand);
        }

        // Unary operators never change the type of the operand, only the value
        symbols.CurrentScope().Annotate(node.ordinal, tOperand);
    }

    void PostorderVisitor::VisitVariableReference(const ast::VariableReference &node)
    {
        symbols.CurrentScope().Annotate(
            node.ordinal,
            symbols.CurrentScope().Lookup(node.name)
        );
    }

    void PostorderVisitor::VisitIntLiteral(const ast::IntLiteral &node)
    {
        symbols.CurrentScope().Annotate(node.ordinal, types::intType);
    }

    void PostorderVisitor::VisitFloatLiteral(const ast::FloatLiteral &node)
    {
        symbols.CurrentScope().Annotate(node.ordinal, types::floatType);
    }

    void PostorderVisitor::VisitBooleanLiteral(const ast::BooleanLiteral &node)
    {
        symbols.CurrentScope().Annotate(node.ordinal, types::boolType);
    }

    void PostorderVisitor::VisitFieldAccess(const ast::FieldAccess &node)
    {
        // Because operators are understood to deal with typed values and field
        // access deals with a value and a name, provide a custom implementation
        // instead of modeling it as an operator
        types::TypeRef tAccessed = symbols.CurrentScope().Lookup(node.accessed->ordinal);
        std::optional<types::TypeRef> tField = types::swizzle(tAccessed, node.field);

        if (!tField)
        {
            node.token.Throw(
                "Field '" + node.field + "' does not exist on type '" +
                types::getTypeName(tAccessed) + "'"
            );
        }

        symbols.CurrentScope().Annotate(node.ordinal, *tField);
    }

    void PostorderVisitor::VisitFunctionCall(const ast::FunctionCall &node)
    {
        types::TypeRef fType = symbols.CurrentScope().Lookup(node.name);
        auto pFunc = std::get_if<std::shared_ptr<types::Function>>(&fType);

        if (pFunc == nullptr)
        {
            node.token.Throw(
                "Symbol '" + node.name + "' is not callable"
            );
        }

        // Lookup type for each argument and verify that they match the
        // function's parameter types. Because types are shared, and TypeRef is
        // backed by a pointer, we only need to perform a pointer-wise
        // comparison to check if types are equal.
        std::vector<types::TypeRef> args;
        args.reserve(node.args.size());

        for (const std::unique_ptr<ast::Expr> &arg : node.args)
        {
            args.push_back(symbols.CurrentScope().Lookup(arg->ordinal));
        }

        if (args != (*pFunc)->params)
        {
            bool pass = false;

            for (const std::vector<types::TypeRef> overload : (*pFunc)->overloads)
            {
                if (args == overload)
                {
                    pass = true;
                    break;
                }
            }

            // TODO: Improve error message for overloaded functions
            if (!pass)
            {
                node.token.Throw(
                    "Wrong argument types for function of type '" +
                    types::getTypeName(fType) + "'"
                );
            }
        }

        symbols.CurrentScope().Annotate(node.ordinal, (*pFunc)->returnType);
    }

    void PostorderVisitor::VisitReturnStat(const ast::ReturnStat &node)
    {
        types::TypeRef tExpr = symbols.CurrentScope().Lookup(node.expr->ordinal);
        std::optional<types::TypeRef> tReturn = symbols.CurrentScope().ReturnType();

        if (!tReturn)
        {
            node.token.Throw("Return statement cannot appear outside of a function body");
        }

        if (tReturn != tExpr)
        {
            node.token.Throw(
                "Return type does not match function definition, "
                "expecting '" + types::getTypeName(*tReturn) +
                "', got '" + types::getTypeName(tExpr) + "'"
            );
        }
    }

    void PostorderVisitor::VisitDeclStat(const ast::DeclStat &node)
    {
        types::TypeRef tLeft = types.Get(node.type);
        symbols.CurrentScope().Declare(node.name, tLeft);

        if (node.initializer)
        {
            types::TypeRef tRight = symbols.CurrentScope().Lookup(node.initializer->ordinal);
            resolveBinaryOperation(node.token, operators::Assign, tLeft, tRight);
        }
    }

    void PostorderVisitor::VisitPropertyDecl(const ast::PropertyDecl &node)
    {
        VisitDeclStat(node);
    }

    void PostorderVisitor::VisitSharedDecl(const ast::SharedDecl &node)
    {
        VisitDeclStat(node);
    }

    void PreorderVisitor::VisitShaderBlock(const ast::ShaderBlock &node)
    {
        symbols.BeginScope(types::fVecTypes[2]);
    }

    void PostorderVisitor::VisitShaderBlock(const ast::ShaderBlock &node)
    {
        symbols.EndScope();
    }

    void PreorderVisitor::VisitRequireBlock(const ast::RequireBlock &node)
    {
        symbols.BeginScope();
    }

    void PostorderVisitor::VisitRequireBlock(const ast::RequireBlock &node)
    {
        symbols.EndScope();
    }

    Traverser::Traverser(types::SymbolTable &symbols, types::TypeRegistry &types)
        : pre(PreorderVisitor(symbols, types))
        , post(PostorderVisitor(symbols, types))
        { }

    void Traverser::Pre(const ast::Node &node)
    {
        node.Dispatch(pre);
    }

    void Traverser::Post(const ast::Node &node)
    {
        node.Dispatch(post);
    }
}
