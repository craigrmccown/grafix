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

TEST_CASE("simple expression", "[slim]")
{
    std::vector<std::unique_ptr<slim::regex::Node>> exprs;
    exprs.push_back(slim::regex::Parse("abc"));

    slim::nfa::Nfa nfa(exprs);
    const slim::nfa::State *state = nfa.GetHead();
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
