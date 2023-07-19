#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "alphabet.hpp"
#include "regex.hpp"

namespace slim::nfa
{
    namespace constants
    {
        // Marks a transition representing all ranges not in the alphabet
        inline constexpr int negative = -1;

        // Marks an empty string transition that moves the NFA into multiple,
        // simultaneous states
        inline constexpr int epsilon = -2;
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
        // Transitions will be sorted by alphabet index after NFA construction.
        // Epsilon transitions always come first.
        std::vector<std::shared_ptr<Transition>> transitions;

        // Will be negative for non-accepting states
        int token;

        // Used to identify the state within the NFA
        int n;

        State(int n, int token);

        // Creates a new transition to the specified state
        std::shared_ptr<Transition> TransitionTo(int iAlphabet, State *to);
    };

    // Performs a depth-first traversal over the NFA graph. Each state will be
    // visited exactly once.
    void Traverse(
        std::vector<State *> states,
        std::function<void (State *)> visit,
        std::function<bool (const Transition &)> follow
    );

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
        Nfa(const Alphabet &alpha, const std::vector<std::unique_ptr<regex::Node>> &exprs);
        ~Nfa();

        // Copy construction and assignment are deleted out of pure laziness.
        // Ideally, we would implement copy semantics by cloning the state
        // machine graph. It is, however, convenient that State pointers can be
        // used to identify states because they will never be moved or copied
        // (e.g. we can keep a set of visited pointers during traversal)
        Nfa(const Nfa &other) = delete;
        Nfa &operator=(const Nfa &other) = delete;

        int Size() const;
        State *GetHead() const;

        private:
        int size;
        State *head;

        State *newState();
        State *newState(int token);
        Partial build(const regex::Node &expr, const Alphabet &alphabet);
    };
}
