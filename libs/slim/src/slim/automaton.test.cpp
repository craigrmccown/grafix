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

    // 1-1, 2-3, 4-5, 6-9, A-C
    slim::nfa::Alphabet ab(buf);
    CHECK(ab.Length() == 5);

    slim::nfa::Alphabet::RangeIndex ri;

    // 1-1, 2-3
    ri = ab.Map('1', '3');
    CHECK((ri.i == 0 && ri.len == 2));

    // 6-9
    ri = ab.Map('6', '9');
    CHECK((ri.i == 3 && ri.len == 1));

    // 2-3, 4-5, 6-9
    ri = ab.Map('2', '9');
    CHECK((ri.i == 1 && ri.len == 3));

    // A-C
    ri = ab.Map('A', 'C');
    CHECK((ri.i == 4 && ri.len == 1));
}

TEST_CASE("alphabet with duplicate ranges", "[slim]")
{
    slim::nfa::Alphabet::Buffer buf;
    buf.Write('1', '9');
    buf.Write('1', '5');
    buf.Write('5', '9');
    buf.Write('5', '9');
    buf.Write('1', '9');

    // 1-4, 5-5, 6-9
    slim::nfa::Alphabet ab(buf);
    CHECK(ab.Length() == 3);

    slim::nfa::Alphabet::RangeIndex ri;

    // 1-4, 5-5, 6-9
    ri = ab.Map('1', '9');
    CHECK((ri.i == 0 && ri.len == 3));

    // 1-4, 5-5
    ri = ab.Map('1', '5');
    CHECK((ri.i == 0 && ri.len == 2));

    // 5-5, 6-9
    ri = ab.Map('5', '9');
    CHECK((ri.i == 1 && ri.len == 2));
}

TEST_CASE("alphabet mapping invalid ranges", "[slim]")
{
    slim::nfa::Alphabet::Buffer buf;
    buf.Write('2', '1'); // Has no effect
    buf.Write('2', '3');
    buf.Write('5', '7');

    // 1-3, 5-7
    slim::nfa::Alphabet ab(buf);
    CHECK(ab.Length() == 2);
    CHECK_THROWS(ab.Map('1', '2'));
    CHECK_THROWS(ab.Map('2', '2'));
    CHECK_THROWS(ab.Map('3', '3'));
    CHECK_THROWS(ab.Map('2', '4'));
    CHECK_THROWS(ab.Map('4', '5'));
    CHECK_THROWS(ab.Map('4', '6'));
    CHECK_THROWS(ab.Map('7', '8'));
    CHECK_THROWS(ab.Map('8', '9'));
    CHECK_THROWS(ab.Map('1', '9'));
}
