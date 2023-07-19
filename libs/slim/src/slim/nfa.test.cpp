#include <memory>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include "regex.hpp"
#include "nfa.hpp"

void checkState(const slim::nfa::State * s, int token, std::vector<int> transitions)
{
    if (token < 0)
    {
        REQUIRE(s->token < 0);
    }
    else
    {
        REQUIRE(s->token == token);
    }

    REQUIRE(transitions.size() == s->transitions.size());

    for (int i = 0; i < transitions.size(); i++)
    {
        REQUIRE(transitions[i] == s->transitions[i]->iAlphabet);
    }
}

TEST_CASE("simple expression", "[slim][nfa]")
{
    std::vector<std::unique_ptr<slim::regex::Node>> exprs;
    exprs.push_back(slim::regex::Parse("abc"));

    slim::Alphabet::Buffer buf;
    buf.Write('a');
    buf.Write('b');
    buf.Write('c');

    slim::nfa::Nfa nfa(slim::Alphabet(buf), exprs);
    REQUIRE(nfa.Size() == 5);

    slim::nfa::State *state = nfa.GetHead();
    checkState(state, -1, {slim::nfa::constants::epsilon});

    state = state->transitions[0]->to;
    checkState(state, -1, {0});

    state = state->transitions[0]->to;
    checkState(state, -1, {1});

    state = state->transitions[0]->to;
    checkState(state, -1, {2});

    state = state->transitions[0]->to;
    checkState(state, 0, {});
}

TEST_CASE("zero or more", "[slim][nfa]")
{
    std::vector<std::unique_ptr<slim::regex::Node>> exprs;
    exprs.push_back(slim::regex::Parse("ab*"));

    slim::Alphabet::Buffer buf;
    buf.Write('a');
    buf.Write('b');

    slim::nfa::Nfa nfa(slim::Alphabet(buf), exprs);
    REQUIRE(nfa.Size() == 5);

    slim::nfa::State *state = nfa.GetHead();
    checkState(state, -1, {slim::nfa::constants::epsilon});

    state = state->transitions[0]->to;
    checkState(state, -1, {0});

    state = state->transitions[0]->to;
    checkState(state, -1, {slim::nfa::constants::epsilon, slim::nfa::constants::epsilon});

    slim::nfa::State
        *left = state->transitions[0]->to,
        *right = state->transitions[1]->to;

    checkState(left, -1, {1});
    REQUIRE(left->transitions[0]->to == state);

    checkState(right, 0, {});
}
