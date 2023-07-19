#pragma once

#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include "nfa.hpp"

namespace slim::dfa
{
    // A set of NFA states formed during powerset construction
    using NfaSubset = std::set<nfa::State *>;

    struct State
    {
        // We use a map to hold transitions for efficient lookups during
        // matching
        std::map<int, State *> transitions;
        int token;

        State(int token); // Only used in tests
        State(const NfaSubset &subset);

        // Creates a new transition to another DFA state
        void TransitionTo(int iAlphabet, State *to);

        // Returns the next state after a transition to the given alphabet
        // index or nullptr if the transition is invalid
        State *GoTo(int iAlphabet);
    };

    // Transforms a DFA state's underlying NFA transitions into a sequence of
    // elements, where each element specifies an alphabet index and a set of
    // destination NFA states. Transitions are grouped such that alphabet
    // indices are guaranteed to be unique in the output.
    class StateIter
    {
        public:
        struct Element
        {
            int iAlphabet;
            std::shared_ptr<NfaSubset> subset;

            Element();
        };

        StateIter(const NfaSubset &subset);

        // Returns false IFF the iterator has been exhausted
        bool Next(Element &out);

        private:
        // Lightweight structure for iterating over the transitions of a single
        // NFA state
        struct NfaStateIter
        {
            std::vector<std::shared_ptr<nfa::Transition>>::const_iterator it, end;

            NfaStateIter(const nfa::State *state);

            // Returns the current transition
            const nfa::Transition &Peek() const;

            // Advances the iterator. If exhausted, returns false.
            bool Advance();

            bool operator >(const NfaStateIter &other) const;
        };

        // A min heap used to merge transitions belonging to many NFA states
        // into a single, sorted view of all transitions
        std::priority_queue<NfaStateIter, std::vector<NfaStateIter>, std::greater<NfaStateIter>> pq;
    };

    class Dfa
    {
        public:
        Dfa(const nfa::Nfa &nfa);
        ~Dfa();

        // See note above deleted nfa::Nfa copy constructor/assignment operator
        Dfa(const Dfa &other) = delete;
        void operator=(const Dfa *other) = delete;

        State *GetHead() const;

        private:
        struct CachedState
        {
            State *s;
            std::shared_ptr<NfaSubset> subset;
        };

        State *head;

        // Caches DFA states by key. See computeKey for specifics around key
        // construction.
        std::map<std::string, CachedState>cache;

        // Creates a new DFA state from a set of NFA states. No assumptions are
        // made about the relationships between the given NFA states. It is up
        // to the caller to construct the right set of NFA states. The number of
        // total NFA states are needed for cache key construction. Repeated
        // calls with the same subset will return a cached value.
        CachedState newState(int totalStates, const NfaSubset &subset);
    };
}
