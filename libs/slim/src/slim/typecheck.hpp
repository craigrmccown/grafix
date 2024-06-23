#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ast.hpp"
#include "types.hpp"

namespace slim::typecheck
{
    class PreorderVisitor : public ast::NoopVisitor
    {
        public:
        PreorderVisitor(types::SymbolTable &symbols, types::TypeRegistry &types);
        void VisitShaderBlock(const ast::ShaderBlock &node) override;
        void VisitRequireBlock(const ast::RequireBlock &node) override;

        private:
        types::SymbolTable &symbols;
        types::TypeRegistry &types;
    };

    class PostorderVisitor : public ast::NoopVisitor
    {
        public:
        PostorderVisitor(types::SymbolTable &symbols, types::TypeRegistry &types);
        void VisitBinaryExpr(const ast::BinaryExpr &node) override;
        void VisitUnaryExpr(const ast::UnaryExpr &node) override;
        void VisitVariableReference(const ast::VariableReference &node) override;
        void VisitIntLiteral(const ast::IntLiteral &node) override;
        void VisitFloatLiteral(const ast::FloatLiteral &node) override;
        void VisitBooleanLiteral(const ast::BooleanLiteral &node) override;
        void VisitFieldAccess(const ast::FieldAccess &node) override;
        void VisitFunctionCall(const ast::FunctionCall &node) override;
        void VisitReturnStat(const ast::ReturnStat &node) override;
        void VisitDeclStat(const ast::DeclStat &node) override;
        void VisitPropertyDecl(const ast::PropertyDecl &node) override;
        void VisitSharedDecl(const ast::SharedDecl &node) override;
        void VisitShaderBlock(const ast::ShaderBlock &node) override;
        void VisitRequireBlock(const ast::RequireBlock &node) override;

        private:
        types::SymbolTable &symbols;
        types::TypeRegistry &types;
    };

    class Traverser : public ast::Traverser
    {
        public:
        Traverser(types::SymbolTable &symbols, types::TypeRegistry &types);

        void Pre(const ast::Node &node);
        void Post(const ast::Node &node);

        private:
        PreorderVisitor pre;
        PostorderVisitor post;
    };
}
