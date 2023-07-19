#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include "dfa.hpp"
#include "nfa.hpp"

namespace slim::dfa
{
    static NfaSubset epsilonClosure(const std::vector<nfa::State *> &states)
    {
        // The closure will also contain every state in the starting set - every
        // state has an implied epsilon transition to itself
        NfaSubset closure;
        nfa::Traverse(
            states,
            [&closure](nfa::State *s) { closure.insert(s); },
            [](const nfa::Transition &t) { return t.iAlphabet == nfa::constants::epsilon; }
        );

        return closure;
    }

    // Computes a key of the specified length, in bytes. Each byte encodes a
    // boolean value signaling whether each state in the source NFA has been
    // grouped into this DFA state. Therefore, the number of states in the
    // source NFA should always be used as the key length by the caller.
    static std::string computeKey(int length, const NfaSubset &subset)
    {
        std::string key(length, '0');
        for (const nfa::State *s : subset) key[s->n] = '1';
        return key;
    }

    State::State(int token) : token(token) { }

    State::State(const NfaSubset &subset)
    {
        token = -1;

        for (const nfa::State *s : subset)
        {
            if (s->token < 0) continue;

            if (token < 0)
            {
                token = s->token;
            }
            else
            {
                // In the case of an ambiguous match, token definition order
                // will be used as a tie breaker. We use std::min so that tokens
                // defined first will have a higher priority.
                token = std::min(s->token, token);
            }
        }
    }

    void State::TransitionTo(int iAlphabet, State *to)
    {
        transitions[iAlphabet] = to;
    }

    State *State::GoTo(int iAlphabet)
    {
        if (transitions.count(iAlphabet)) return transitions[iAlphabet];
        return nullptr;
    }

    StateIter::Element::Element() : subset(std::make_shared<NfaSubset>()) { }

    StateIter::NfaStateIter::NfaStateIter(const nfa::State *state) : it(state->transitions.begin()), end(state->transitions.end()) { }

    const nfa::Transition &StateIter::NfaStateIter::Peek() const
    {
        return **it;
    }

    bool StateIter::NfaStateIter::Advance()
    {
        return it != end && ++it != end;
    }

    bool StateIter::NfaStateIter::operator >(const StateIter::NfaStateIter &other) const
    {
        return Peek().iAlphabet > other.Peek().iAlphabet;
    }

    StateIter::StateIter(const NfaSubset &subset) {
        for (const nfa::State *s : subset)
        {
            // Skip states without transitions - only states with transitions
            // can be compared and indexed into the heap
            if (!s->transitions.size()) continue;

            NfaStateIter iter(s);

            // Advance every iterator past epsilon transitions because they
            // aren't actually part of the alphabet and will not show up in DFA
            // transitions.
            while (true)
            {
                if (iter.Peek().iAlphabet != nfa::constants::epsilon)
                {
                    pq.push(iter);
                    break;
                }

                if (!iter.Advance()) break;
            }
        }
    }

    bool StateIter::Next(StateIter::Element &out)
    {
        if (pq.empty()) return false;

        NfaStateIter iter = pq.top();
        out.iAlphabet = iter.Peek().iAlphabet;
        out.subset->clear();

        // Group states that have like tokens by maintaining a min heap that
        // always produces the next value across all NFA states.
        while (iter.Peek().iAlphabet == out.iAlphabet)
        {
            out.subset->insert(iter.Peek().to);

            // Continuously remove the min iterator, advance it, and heapify
            // until all elements have been consumed across all iterators.
            pq.pop();

            if (iter.Advance())
            {
                pq.push(iter);
            }
            else if (pq.empty())
            {
                break;
            }

            iter = pq.top();
        }

        return true;
    }

    Dfa::CachedState Dfa::newState(int totalStates, const NfaSubset &subset)
    {
        std::string key = computeKey(totalStates, subset);

        if (!cache.count(key))
        {
            cache[key] = CachedState { .s = new State(subset), .subset = std::make_shared<NfaSubset>(subset) };
        }

        return cache[key];
    }

    Dfa::Dfa(const nfa::Nfa &nfa)
    {
        CachedState s = newState(nfa.Size(), epsilonClosure({nfa.GetHead()}));
        head = s.s;

        // Create a stack to hold unprocessed DFA states. When there is no more
        // work to do, we know the DFA is complete. Also mantain a set of
        // visited states to break cycles.
        std::queue<CachedState> q({s});
        std::set<State *> visited({head});

        while (q.size())
        {
            StateIter iter(*q.front().subset);
            StateIter::Element el;

            // For each DFA state, find the set of destination NFA states for
            // each alphabet index. Then, use that state set to find the epsilon
            // closure of that NFA state set. Use that closure to form a new DFA
            // node and transition to it.
            while (iter.Next(el))
            {
                s = newState(
                    nfa.Size(),
                    epsilonClosure(std::vector<nfa::State *>(el.subset->begin(), el.subset->end()))
                );

                q.front().s->TransitionTo(el.iAlphabet, s.s);

                if (!visited.count(s.s))
                {
                    q.push(s);
                    visited.insert(s.s);
                }
            }

            q.pop();
        }
    }

    Dfa::~Dfa()
    {
        for (auto const &[k, v] : cache) delete v.s;
    }

    State *Dfa::GetHead() const
    {
        return head;
    }
}
