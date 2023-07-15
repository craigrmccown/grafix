#pragma once

#include <memory>
#include <vector>
#include "alphabet.hpp"
#include "regex.hpp"

namespace slim::nfa
{
    namespace constants
    {
        // Marks an empty string transition that moves the NFA into multiple,
        // simultaneous states
        inline constexpr int epsilon = -1;

        // Marks a transition representing all ranges not in the alphabet
        inline constexpr int negative = -2;
    }

    struct State;

    // A pointer to a new state with an associated index into the NFA's alphabet
    struct Transition
    {
        // Use raw pointers because there can be cycles, so reference counting
        // won't work
        State *to;

        // Can be negative in certain special cases (e.g. epsilon transitions)
        int iAlphabet;

        Transition(int iAlphabet, State *to);
    };

    // A node in the state machine graph. Holds a collection of transitions to
    // other states.
    struct State
    {
        std::vector<std::shared_ptr<Transition>> transitions;

        // Will be negative for non-accepting states
        int token;

        // Used to identify the state within the NFA
        int n;

        State(int n, int token);

        // Creates a new transition to the specified state
        std::shared_ptr<Transition> TransitionTo(int iAlphabet, State *to);
    };

    // Represents a partial NFA that can be combined with other partial NFAs to
    // form the complete state machine. Points to an entry state and keeps track
    // of dangling transitions reachable from that state that do not yet point
    // to a state.
    struct Partial
    {
        State *head;
        std::vector<std::shared_ptr<Transition>> dangling;

        // Sets all dangling transitions to the given address
        void Chain(State *to);
    };

    // Builds an NFA from a set of expressions. The indices of expressions
    // supplied to the constructor are held by accepting states and should be
    // known to the caller at match time.
    class Nfa
    {
        public:
        Nfa(const std::vector<std::unique_ptr<regex::Node>> &exprs);
        ~Nfa();

        // Copy construction and assignment are deleted out of pure laziness.
        // Ideally, we would implement copy semantics by cloning the state
        // machine graph.
        Nfa(const Nfa &other) = delete;
        Nfa &operator=(const Nfa &other) = delete;

        int Size() const;
        const State *GetHead() const;

        private:
        int size;
        State *head;

        State *newState();
        State *newState(int token);
        Partial build(const regex::Node &expr, const Alphabet &alphabet);
        void free(State *s);
    };
}
