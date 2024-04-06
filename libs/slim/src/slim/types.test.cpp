#include <catch2/catch_test_macros.hpp>
#include "types.hpp"

TEST_CASE("symbol table", "[slim]")
{
    slim::types::SymbolTable table;
    const slim::types::Scope &root = table.CurrentScope();

    CHECK_THROWS(table.EndScope());

    table.Associate(0);
    CHECK(&table.ScopeFor(0) == &root);
    CHECK_THROWS(table.ScopeFor(1));

    table.CurrentScope().Declare(2, "sym1", slim::types::TypeInfo{
        .type = slim::types::Type::Int,
    });

    CHECK(table.CurrentScope().Lookup("sym1").type == slim::types::Type::Int);
    CHECK_THROWS(table.CurrentScope().Lookup("sym2"));

    table.BeginScope();
    CHECK(&root != &table.CurrentScope());
    CHECK(table.CurrentScope().Lookup("sym1").type == slim::types::Type::Int);

    table.EndScope();
    CHECK(&root == &table.CurrentScope());
}
