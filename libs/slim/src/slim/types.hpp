#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace slim::types
{
    enum Type {
        Bool,
        Int,
        Uint,
        Float,
        Bvec2,
        Bvec3,
        Bvec4,
        Ivec2,
        Ivec3,
        Ivec4,
        Uvec2,
        Uvec3,
        Uvec4,
        Vec2,
        Vec3,
        Vec4,
        Sampler2d
    };

    // TODO: Support user-defined types, including structs
    struct TypeInfo
    {
        const Type type;

        // Used for composite types, such as functions
        const std::vector<Type> types;
    };

    // Represens a lexical scope. Holds type information for unnamed
    // expressions, referred to by ID, and named expressions, referred to by
    // name. IDs are treated as opaque, so no assumptions are made about their
    // structure or relation to one another.
    class Scope
    {
        public:
        Scope(Scope *parent = nullptr);

        // Associates an unnamed identifier with a type
        void Annotate(uint32_t id, const TypeInfo &type);

        // Associates a named identifier with a type. Names are treated as an
        // additional way to access types, but the type is still annotated by
        // ID as well.
        void Declare(uint32_t id, const std::string &sym, const TypeInfo &type);

        // Retrieves type information by ID. Does not attempt to find type
        // information in ancestor scopes.
        const TypeInfo &Lookup(uint32_t id) const;

        // Retrieves type information by name. If the current scope has no entry
        // for the given name, ancestor scopes are queried until either the name
        // is found or the root scope is reached.
        const TypeInfo &Lookup(const std::string &sym) const;

        private:
        Scope *parent;
        std::map<uint32_t, std::unique_ptr<TypeInfo>> types;
        std::map<std::string, uint32_t> symbols;
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
}
