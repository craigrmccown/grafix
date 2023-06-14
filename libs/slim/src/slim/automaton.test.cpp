#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "automaton.hpp"

TEST_CASE("alphabet with overlapping ranges", "[slim]")
{
    slim::nfa::Alphabet::Buffer buf;
    buf.Write('1', '3');
    buf.Write('6', '9');
    buf.Write('2', '9');
    buf.Write('A', 'C');

    slim::nfa::Alphabet ab(buf);
}
