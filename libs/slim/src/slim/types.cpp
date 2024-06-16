#include <array>
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
    const std::shared_ptr<Scalar> boolType = std::make_shared<Scalar>(Scalar::Type::Bool);
    const std::shared_ptr<Scalar> intType = std::make_shared<Scalar>(Scalar::Type::Int);
    const std::shared_ptr<Scalar> uintType = std::make_shared<Scalar>(Scalar::Type::Uint);
    const std::shared_ptr<Scalar> floatType = std::make_shared<Scalar>(Scalar::Type::Float);
    const std::array<std::shared_ptr<Vector>, 3> bVecTypes = {
        std::make_shared<Vector>(Scalar::Bool, 2),
        std::make_shared<Vector>(Scalar::Bool, 3),
        std::make_shared<Vector>(Scalar::Bool, 4),
    };
    const std::array<std::shared_ptr<Vector>, 3> iVecTypes = {
        std::make_shared<Vector>(Scalar::Int, 2),
        std::make_shared<Vector>(Scalar::Int, 3),
        std::make_shared<Vector>(Scalar::Int, 4),
    };
    const std::array<std::shared_ptr<Vector>, 3> uVecTypes = {
        std::make_shared<Vector>(Scalar::Uint, 2),
        std::make_shared<Vector>(Scalar::Uint, 3),
        std::make_shared<Vector>(Scalar::Uint, 4),
    };
    const std::array<std::shared_ptr<Vector>, 3> fVecTypes = {
        std::make_shared<Vector>(Scalar::Float, 2),
        std::make_shared<Vector>(Scalar::Float, 3),
        std::make_shared<Vector>(Scalar::Float, 4),
    };

    Scalar::Scalar(Type type) : type(type) { }

    Vector::Vector(Scalar::Type type, u_int8_t length) : type(type), length(length) { }

    TypeRef Vector::GetUnderlyingType() const
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

    static std::array<std::shared_ptr<Vector>, 3> getVectorTypes(Scalar::Type type)
    {
        switch (type)
        {
            case Scalar::Bool:
                return bVecTypes;
            case Scalar::Int:
                return iVecTypes;
            case Scalar::Uint:
                return uVecTypes;
            case Scalar::Float:
                return fVecTypes;
        }
    }

    std::shared_ptr<Vector> getVectorType(Scalar::Type type, u_int8_t length)
    {
        // There is an implicit dependency between the length of array types and
        // their positions in the const arrays that hold them, which is codified
        // in this assertion.
        assert(length >= 2 && length <= 4);
        return getVectorTypes(type)[length - 2];
    }

    static std::string getVectorTypeName(Vector vec)
    {
        std::string name = "vec" + std::to_string(vec.length);

        switch (vec.type)
        {
            case types::Scalar::Bool:
                return "b" + name;
            case types::Scalar::Int:
                return "i" + name;
            case types::Scalar::Uint:
                return "u" + name;
            case types::Scalar::Float:
                return name;
        }
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
            return getVectorTypeName(**p);
        }
        else if (auto p = std::get_if<std::shared_ptr<Matrix>>(&type))
        {
            return "mat" + std::to_string((*p)->size);
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

    static bool isValidSwizzleMask(const std::string &s)
    {
        if (s.length() == 0 || s.length() > 4) return false;

        uint8_t mask = 0;

        // Check that all characters in the swizzle mask belong to the same
        // grouping of valid characters. Note that no valid characters use
        // variable length encoding, so we can iterate over each byte.
        for (char c : s)
        {
            switch (c)
            {
                case 'x':
                case 'y':
                case 'z':
                case 'w':
                    mask = mask | 1;
                    break;
                case 'r':
                case 'g':
                case 'b':
                case 'a':
                    mask = mask | 2;
                    break;
                case 's':
                case 't':
                case 'p':
                case 'q':
                    mask = mask | 4;
                    break;
            }

            // Check that only a single bit is set
            if ((mask & (mask - 1)) != 0) return false;
        }

        return true;
    }

    static bool swizzleMaskInRange(const std::string &s, uint8_t length)
    {
        for (char c : s)
        {
            switch (c)
            {
                case 'x':
                case 'r':
                case 's':
                    if (length < 1) return false;
                    break;
                case 'y':
                case 'g':
                case 't':
                    if (length < 2) return false;
                    break;
                case 'z':
                case 'b':
                case 'p':
                    if (length < 3) return false;
                    break;
                case 'w':
                case 'a':
                case 'q':
                    if (length < 4) return false;
                    break;
            }
        }

        return true;
    }

    std::optional<TypeRef> swizzle(TypeRef type, const std::string &s)
    {
        if (auto p = std::get_if<std::shared_ptr<Vector>>(&type))
        {
            if (!isValidSwizzleMask(s)) return std::nullopt;
            else if (!swizzleMaskInRange(s, (*p)->length)) return std::nullopt;

            u_int8_t length = s.length();

            if (length == (*p)->length) return type;
            else if (length == 1) return (*p)->GetUnderlyingType();

            return getVectorType((*p)->type, length);
        }

        return std::nullopt;
    }

    TypeRef TypeRegistry::Get(const std::string &name) const
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

    bool TypeRegistry::Has(const std::string &name) const
    {
        return types.find(name) != types.end();
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
        const std::vector<const std::shared_ptr<Scalar>> scalarTypes = { boolType, intType, uintType, floatType };

        for (const std::shared_ptr<Scalar> &type : scalarTypes)
        {
            types.Define(type);

            // All scalar types can be converted to one another by explicitly
            // calling a type constructor function. Create type constructor
            // functions for each scalar type, and overload them to accept each
            // other type
            std::vector<const std::vector<TypeRef>> overloads;
            overloads.reserve(scalarTypes.size() - 1);

            for (const auto &otherType : scalarTypes)
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
            auto vecTypes = getVectorTypes(type->type);

            for (int i = 0; i < 3; i++)
            {
                // Define a vector type for each combination of scalar type and
                // length
                uint8_t length = i + 2;
                std::shared_ptr<Vector> vecType = vecTypes[i];
                types.Define(vecType);

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

        // Define matrix types and constructor functions
        std::array<TypeRef, 3> matTypes;

        for (int i = 0; i < 3; i++)
        {
            uint8_t size = i + 2;
            std::shared_ptr<Matrix> type = std::make_shared<Matrix>(Matrix{ .size = size });

            types.Define(type);
            matTypes[i] = type;
        }

        for (const TypeRef &type : matTypes)
        {
            // Every matrix type can be constructed from every other matrix
            // type, where values will either be truncated or receive default
            // values from the corresponding location in the identity matrix
            std::vector<const std::vector<TypeRef>> overloads;
            overloads.reserve(matTypes.size());

            for (const TypeRef &otherType : matTypes)
            {
                overloads.push_back({ otherType });
            }

            // Matrix types can also be constructed from a single float vlaue,
            // which will create a diagonal matrix using that value
            auto fType = std::make_shared<Function>(Function{
                .returnType = type,
                .name = getTypeName(type),
                .params = { types::floatType },
                .overloads = overloads,
            });

            symbols.CurrentScope().Declare(fType->name, fType);
        }

        symbols.BeginScope();
    }
}
