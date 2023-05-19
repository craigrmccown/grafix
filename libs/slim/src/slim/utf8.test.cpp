#include <string>
#include <catch2/catch_test_macros.hpp>
#include "utf8.hpp"

TEST_CASE("ascii as UTF8", "[slim]") {
    std::string input = "abcde";
    slim::utf8::Decoder decoder(input.begin(), input.end());

    int i = 0;
    slim::utf8::Glyph g;
    while (decoder.Next(g))
    {
        REQUIRE(input[i++] == g);
    }
    REQUIRE(!decoder.Err());
}
