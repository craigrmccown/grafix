#include <set>
#include <catch2/catch_test_macros.hpp>
#include "alphabet.hpp"
#include "nfa.hpp"
#include "dfa.hpp"

static bool statesEqual(
    slim::dfa::State *a,
    slim::dfa::State *b,
    std::set<slim::dfa::State *> &visited
)
{
    if (a->token != b->token) return false;
    if (a->transitions.size() != b->transitions.size()) return false;

    visited.insert(a);

    for (auto const &[k, av] : a->transitions)
    {
        slim::dfa::State *bv = b->GoTo(k);

        if (!bv) return false;
        if (!visited.count(av) && !statesEqual(av, bv, visited)) return false;
    }

    return true;
}

TEST_CASE("iteration over NFA state transitions", "[slim][dfa]")
{
    slim::nfa::State
        s0(0, -1),
        s1(1, -1),
        s2(2, -1),
        s3(3, -1),
        s4(4, -1),
        s5(5, -1),
        s6(6, -1),
        s7(7, -1);

    s0.TransitionTo(slim::nfa::constants::epsilon, &s6);
    s0.TransitionTo(slim::nfa::constants::epsilon, &s7);
    s0.TransitionTo(0, &s3);
    s0.TransitionTo(1, &s4);
    s1.TransitionTo(0, &s5);
    s1.TransitionTo(2, &s4);
    s2.TransitionTo(slim::nfa::constants::epsilon, &s6);
    s2.TransitionTo(1, &s4);
    s2.TransitionTo(3, &s5);

    slim::dfa::StateIter iter({&s0, &s1, &s2});
    slim::dfa::StateIter::Element el;

    REQUIRE(iter.Next(el));
    REQUIRE(el.iAlphabet == 0);
    REQUIRE(el.subset->size() == 2);
    REQUIRE(el.subset->count(&s3));
    REQUIRE(el.subset->count(&s5));

    REQUIRE(iter.Next(el));
    REQUIRE(el.iAlphabet == 1);
    REQUIRE(el.subset->size() == 1);
    REQUIRE(el.subset->count(&s4));

    REQUIRE(iter.Next(el));
    REQUIRE(el.iAlphabet == 2);
    REQUIRE(el.subset->size() == 1);
    REQUIRE(el.subset->count(&s4));

    REQUIRE(iter.Next(el));
    REQUIRE(el.iAlphabet == 3);
    REQUIRE(el.subset->size() == 1);
    REQUIRE(el.subset->count(&s5));
}

TEST_CASE("overlapping range", "[slim][dfa]")
{
    std::vector<std::unique_ptr<slim::regex::Node>> exprs;
    exprs.push_back(slim::regex::Parse("ab"));
    exprs.push_back(slim::regex::Parse("[a-z]+"));

    slim::Alphabet::Buffer buf;
    buf.Write('a');
    buf.Write('b');
    buf.Write('c', 'z');

    slim::dfa::Dfa dfa(slim::nfa::Nfa(slim::Alphabet(buf), exprs));

    slim::dfa::State s1(-1), s2(1), s3(1), s4(0);
    s1.TransitionTo(0, &s2);
    s1.TransitionTo(1, &s3);
    s1.TransitionTo(2, &s3);
    s2.TransitionTo(0, &s3);
    s2.TransitionTo(1, &s4);
    s2.TransitionTo(2, &s3);
    s3.TransitionTo(0, &s3);
    s3.TransitionTo(1, &s3);
    s3.TransitionTo(2, &s3);
    s4.TransitionTo(0, &s3);
    s4.TransitionTo(1, &s3);
    s4.TransitionTo(2, &s3);

    std::set<slim::dfa::State *> visited;
    REQUIRE(statesEqual(&s1, dfa.GetHead(), visited));
}
