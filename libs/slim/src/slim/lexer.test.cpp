#include <catch2/catch_test_macros.hpp>
#include "lexer.hpp"

TEST_CASE("myfunc", "[slim]") {
    slim::myfunc();
    REQUIRE(true);
}
