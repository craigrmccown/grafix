#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include "types.hpp"

namespace slim::types
{
    Scope::Scope(Scope *parent) : parent(parent) { }

    void Scope::Annotate(uint32_t id, const TypeInfo &type)
    {
        if (types.find(id) != types.end())
        {
            throw std::runtime_error(
                "ID " +
                std::to_string(id) +
                " has already been annotated with a type"
            );
        }

        types[id] = std::make_unique<TypeInfo>(type);
    }

    void Scope::Declare(
        uint32_t id,
        const std::string &sym,
        const TypeInfo &type
    )
    {
        if (symbols.find(sym) != symbols.end())
        {
            throw std::runtime_error(
                "Symbol '"
                + sym +
                "' has already been defined"
            );
        }

        Annotate(id, type);
        symbols[sym] = id;
    }

    const TypeInfo &Scope::Lookup(uint32_t id) const
    {
        auto it = types.find(id);

        if (it == types.end())
        {
            throw std::runtime_error(
                "ID " +
                std::to_string(id) +
                " not associated."
            );
        }

        return *(*it).second;
    }

    const TypeInfo &Scope::Lookup(const std::string &sym) const
    {
        auto it = symbols.find(sym);

        if (it == symbols.end())
        {
            // The root scope has been reached, and the symbol was not found
            if (parent == nullptr) {
                throw std::runtime_error("Symbol '" + sym + "' is not defined");
            }

            // Recursively query ancestor scopes until the symbol is found
            return parent->Lookup(sym);
        }

        return Lookup((*it).second);
    }

    SymbolTable::SymbolTable()
    {
        pushScope(nullptr);
    }

    void SymbolTable::BeginScope()
    {
        pushScope(&CurrentScope());
    }

    void SymbolTable::EndScope()
    {
        assert(stack.size() != 0);

        if (stack.size() == 1)
        {
            throw std::runtime_error("Root scope cannot be ended");
        }

        stack.pop_back();
    }

    void SymbolTable::Associate(uint32_t id)
    {
        if (associations.find(id) != associations.end())
        {
            throw std::runtime_error(
                "ID " +
                std::to_string(id) +
                " has already been associated with a scope and cannot be" +
                " reassociated"
            );
        }

        associations[id] = &CurrentScope();
    }

    Scope &SymbolTable::CurrentScope() const
    {
        return *stack.back();
    }

    const Scope &SymbolTable::ScopeFor(uint32_t id) const
    {
        auto it = associations.find(id);

        if (it == associations.end())
        {
            throw std::runtime_error(
                "ID " +
                std::to_string(id) +
                " is not associated with any scope"
            );
        }

        return *(*it).second;
    }

    void SymbolTable::pushScope(Scope *parent)
    {
        scopes.push_back(std::make_unique<Scope>(parent));
        stack.push_back(&*scopes.back());
    }
}
