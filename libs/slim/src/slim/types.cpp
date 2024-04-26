#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include "types.hpp"

namespace slim::types
{
    const TypeRef boolType = std::make_shared<Scalar>(Scalar::Type::Bool);
    const TypeRef intType = std::make_shared<Scalar>(Scalar::Type::Int);
    const TypeRef uintType = std::make_shared<Scalar>(Scalar::Type::Uint);
    const TypeRef floatType = std::make_shared<Scalar>(Scalar::Type::Float);

    Scalar::Scalar(Type type) : type(type) { }

    const TypeRef &Vector::GetUnderlyingType() const
    {
        switch (type)
        {
            case Scalar::Type::Bool:
                return boolType;
            case Scalar::Type::Int:
                return intType;
            case Scalar::Type::Uint:
                return uintType;
            case Scalar::Type::Float:
                return floatType;
        }
    }

    std::optional<TypeRef> Vector::AccessProperty(const std::string &prop) const
    {
        // TODO: Implement swizzling
        return std::nullopt;
    }

    std::string getTypeName(TypeRef type)
    {
        if (auto p = std::get_if<std::shared_ptr<Opaque>>(&type))
        {
            switch ((*p)->type)
            {
                case types::Opaque::Sampler2d:
                    return "sampler2D";
            }
        }
        else if (auto p = std::get_if<std::shared_ptr<Scalar>>(&type))
        {
            switch ((*p)->type)
            {
                case types::Scalar::Bool:
                    return "bool";
                case types::Scalar::Int:
                    return "int";
                case types::Scalar::Uint:
                    return "uint";
                case types::Scalar::Float:
                    return "float";
            }
        }
        else if (auto p = std::get_if<std::shared_ptr<Vector>>(&type))
        {
            std::string postfix = "vec" + std::to_string((*p)->length);

            switch ((*p)->type)
            {
                case types::Scalar::Bool:
                    return "b" + postfix;
                case types::Scalar::Int:
                    return "i" + postfix;
                case types::Scalar::Uint:
                    return "u" + postfix;
                case types::Scalar::Float:
                    return postfix;
            }
        }
        else if (auto p = std::get_if<std::shared_ptr<Function>>(&type))
        {
            std::string name = getTypeName((*p)->returnType) + " " + (*p)->name + "(";
            auto begin = (*p)->params.begin(), end = (*p)->params.end();

            if (begin != end)
            {
                name += getTypeName(*begin);
            }

            while (++begin != end)
            {
                name += "," + getTypeName(*begin);
            }

            return name + ")";
        }

        // TODO: Use std::visit instead of if/else to operate on each type for
        // compile-time exhaustiveness check
        throw std::runtime_error("Unrecognized type");
    }

    bool isIntegerType(Scalar::Type type)
    {
        return type == Scalar::Int || type == Scalar::Uint;
    }

    bool isNumericType(Scalar::Type type)
    {
        return isIntegerType(type) || type == Scalar::Float;
    }

    TypeRef TypeRegistry::Get(const std::string &name)
    {
        auto it = types.find(name);

        if (it == types.end())
        {
            throw std::runtime_error(
                "Type with name '" + name + "' is not defined"
            );
        }

        return (*it).second;
    }

    void TypeRegistry::Define(TypeRef type)
    {
        std::string name = getTypeName(type);

        if (types.find(name) != types.end())
        {
            throw std::runtime_error(
                "Type with name '" + name + "' has already been defined"
            );
        }

        types[name] = type;
    }

    Scope::Scope(Scope *parent) : parent(parent) { }

    void Scope::Annotate(uint32_t id, const TypeRef &type)
    {
        if (types.find(id) != types.end())
        {
            throw std::runtime_error(
                "ID " +
                std::to_string(id) +
                " has already been annotated with a type"
            );
        }

        types[id] = type;
    }

    void Scope::Declare(
        const std::string &sym,
        const TypeRef &type
    )
    {
        if (symbols.find(sym) != symbols.end())
        {
            throw std::runtime_error(
                "Symbol '"
                + sym +
                "' has already been declared"
            );
        }

        symbols[sym] = type;
    }

    const TypeRef &Scope::Lookup(uint32_t id) const
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

        return (*it).second;
    }

    const TypeRef &Scope::Lookup(const std::string &sym) const
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

        return (*it).second;
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

    void initializeBuiltIns(SymbolTable &symbols, TypeRegistry &types)
    {
        // Opaque types cannot be composed with other types, so we can define
        // them independently
        types.Define(std::make_shared<Opaque>(Opaque{ .type = Opaque::Sampler2d }));

        // Scalar types are defined next, along with their corresponding vector
        // types and type constructor functions
        const std::vector<TypeRef> scalarTypes = { boolType, intType, uintType, floatType };

        for (const TypeRef &type : scalarTypes)
        {
            types.Define(type);

            // All scalar types can be converted to one another by explicitly
            // calling a type constructor function. Create type constructor
            // functions for each scalar type, and overload them to accept each
            // other type
            std::vector<const std::vector<TypeRef>> overloads;
            overloads.reserve(scalarTypes.size() - 1);

            for (const TypeRef &otherType : scalarTypes)
            {
                if (otherType != type) overloads.push_back({ otherType });
            }

            symbols.CurrentScope().Declare(
                getTypeName(type),
                std::make_shared<Function>(Function {
                    .returnType = type,
                    .params = { type },
                    .overloads = overloads,
                })
            );

            // Every scalar type defines 3 corresponding vector types whose
            // lengths range from 2 to 4
            std::shared_ptr<Vector> vecTypes[3];

            for (int i = 0; i < 3; i++)
            {
                // Define a vector type for each combination of scalar type and
                // length
                uint8_t length = i + 2;

                std::shared_ptr<Vector> vecType = std::make_shared<Vector>(Vector{
                    .type = std::get<std::shared_ptr<Scalar>>(type)->type,
                    .length = length,
                });

                types.Define(vecType);

                // Keep track of defined vector types of each length - they are
                // needed to build type constructor function overloads
                vecTypes[i] = vecType;

                // Build the default constructor parameters for each vector
                // type, which take the form of vecN(x0, ..., xN), where each
                // parameter takes the underlying type of the vector
                //
                // Example:
                //      vec3(float, float, float)
                std::vector<TypeRef> params;
                params.reserve(length);
                for (int j = 0; j < params.capacity(); j++) params.push_back(type);

                // Build overloads for each vector constructor function. There
                // are two types of overload. One takes the form vecN(x), where
                // the parameter x takes the underlying type of the vector. The
                // other takes the form vecN(vecM, x0, ..., xN-M), where M is
                // less than or equal to N.
                //
                // Example:
                //      vec4(float)
                //      vec4(vec2, float, float)
                //      vec4(vec3, float)
                //      vec4(vec4)
                std::vector<const std::vector<TypeRef>> overloads;
                overloads.reserve(length);
                overloads.push_back({ type });
                for (int j = 0; j < length - 1; j++)
                {
                    std::vector<TypeRef> overload;
                    overload.reserve(length - j - 1);
                    overload.push_back(vecTypes[j]);
                    for (int k = 1; k < overload.capacity(); k++) overload.push_back(type);

                    overloads.push_back(overload);
                }

                auto fType = std::make_shared<Function>(Function{
                    .returnType = vecType,
                    .name = getTypeName(vecType),
                    .params = params,
                    .overloads = overloads,
                });

                symbols.CurrentScope().Declare(fType->name, fType);
            }
        }

        symbols.BeginScope();
    }
}
