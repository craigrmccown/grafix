#include <catch2/catch_test_macros.hpp>
#include "regex.hpp"

TEST_CASE("foo", "[slim]")
{
    std::string pattern = "abc(a|b)";
    std::unique_ptr<slim::regex::Node> tree = slim::regex::parse(pattern);
}
