#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace slim::types
{
    struct Opaque;
    struct Scalar;
    struct Vector;
    struct Function;

    using TypeRef = std::variant<
        std::shared_ptr<Opaque>,
        std::shared_ptr<Scalar>,
        std::shared_ptr<Vector>,
        std::shared_ptr<Function>
    >;

    struct Opaque
    {
        enum Type
        {
            Sampler2d
        };

        const Type type;
    };

    struct Scalar
    {
        enum Type
        {
            Bool,
            Int,
            Uint,
            Float,
        };

        const Type type;

        Scalar(Type type);
    };

    struct Vector
    {
        const Scalar::Type type;
        const u_int8_t length;

        const TypeRef &GetUnderlyingType() const;
    };

    struct Function
    {
        const TypeRef returnType;
        const std::string name;
        const std::vector<TypeRef> params;

        // Function overloads are allowed for built-in functions
        const std::vector<const std::vector<TypeRef>> overloads;
    };

    // Scalar types can be referenced directly instead of only through a type
    // registry
    extern const TypeRef boolType;
    extern const TypeRef intType;
    extern const TypeRef uintType;
    extern const TypeRef floatType;

    std::string getTypeName(TypeRef type);
    bool isIntegerType(Scalar::Type type);
    bool isNumericType(Scalar::Type type);

    // Holds system and user-defined types. Supports a nominal type system,
    // where each type is addressable by name. Type names are always global.
    class TypeRegistry
    {
        public:
        TypeRef Get(const std::string &name) const;
        bool Has(const std::string &name) const;
        void Define(TypeRef type);

        private:
        std::map<std::string, TypeRef> types;
    };

    std::optional<TypeRef> swizzle(
        TypeRef type,
        const TypeRegistry &types,
        const std::string &s
    );

    // Represents a lexical scope. Holds type information for unnamed
    // expressions, referred to by ID, and named expressions, referred to by
    // name. IDs are treated as opaque, so no assumptions are made about their
    // structure or relation to one another.
    class Scope
    {
        public:
        Scope(Scope *parent = nullptr);

        // Associates an unnamed identifier with a type
        void Annotate(uint32_t id, const TypeRef &type);

        // Associates a named identifier with a type
        void Declare(const std::string &sym, const TypeRef &type);

        // Retrieves type information by ID. Does not attempt to find type
        // information in ancestor scopes.
        const TypeRef &Lookup(uint32_t id) const;

        // Retrieves type information by name. If the current scope has no entry
        // for the given name, ancestor scopes are queried until either the name
        // is found or the root scope is reached.
        const TypeRef &Lookup(const std::string &sym) const;

        private:
        Scope *parent;
        std::map<uint32_t, TypeRef> types;
        std::map<std::string, TypeRef> symbols;
    };

    // Stateful class for building a symbol table with lexical scoping. Scopes
    // hold type information about typed expressions, while the symbol table
    // holds the scopes. A layer of indirection, the ID, exists to decouple the
    // symbol table from any concrete parser implementation.
    class SymbolTable
    {
        public:
        SymbolTable();

        // Creates a new child scope relative to the current scope
        void BeginScope();

        // Ends the current scope and resets the current scope to its parent
        void EndScope();

        // Associates an ID with the current scope. The semantic meaning of the
        // ID is determined by the caller, but it is meant to refer to a typed
        // expression.
        void Associate(uint32_t id);

        // Returns the current scope
        Scope &CurrentScope() const;

        // Returns the scope associated with the given ID
        const Scope &ScopeFor(uint32_t id) const;

        private:
        std::vector<std::unique_ptr<Scope>> scopes;
        std::map<uint32_t, Scope*> associations;
        std::vector<Scope*> stack;

        void pushScope(Scope *parent);
    };

    // Registers symbols and types provided by the language runtime
    void initializeBuiltIns(SymbolTable &symbols, TypeRegistry &types);
}
