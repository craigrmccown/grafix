#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <variant>
#include "types.hpp"

TEST_CASE("symbol table", "[slim]")
{
    slim::types::SymbolTable table;
    const slim::types::Scope &root = table.CurrentScope();

    CHECK_THROWS(table.EndScope());

    table.Associate(0);
    CHECK(&table.ScopeFor(0) == &root);
    CHECK_THROWS(table.ScopeFor(1));

    // Declare sym1 as an int and retrieve from symbol table
    table.CurrentScope().Declare("sym1", slim::types::intType);
    slim::types::TypeRef type = table.CurrentScope().Lookup("sym1");
    auto scalar = std::get_if<std::shared_ptr<slim::types::Scalar>>(&type);

    // Check that the looked up type is an int and that an undefined symbol
    // cannot be looked up
    CHECK((scalar && (*scalar)->type == slim::types::Scalar::Type::Int));
    CHECK_THROWS(table.CurrentScope().Lookup("sym2"));

    // Begin a new scope and verify a new scope has been created
    table.BeginScope();
    CHECK(&root != &table.CurrentScope());

    // Look up sym1 from the new scope and verify it can still be accessed
    type = table.CurrentScope().Lookup("sym1");
    scalar = std::get_if<std::shared_ptr<slim::types::Scalar>>(&type);
    CHECK((scalar && (*scalar)->type == slim::types::Scalar::Type::Int));

    // End scope and verify we are back to the root
    table.EndScope();
    CHECK(&root == &table.CurrentScope());
}
