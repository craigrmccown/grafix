#include <memory>
#include <set>
#include <stack>
#include <stdexcept>
#include <vector>
#include "alphabet.hpp"
#include "nfa.hpp"
#include "regex.hpp"

namespace slim::nfa
{
    static void extractAlphabet(const regex::Node &expr, Alphabet::Buffer &buf)
    {
        switch (expr.kind)
        {
            case regex::Node::Literal:
                buf.Write(expr.g);
                break;
            case regex::Node::Range:
                buf.Write(expr.left->g, expr.right->g);
                break;
            default:
                extractAlphabet(*expr.left, buf);
                extractAlphabet(*expr.right, buf);
                break;
        }
    }

    Transition::Transition(int iAlphabet, State *to) : iAlphabet(iAlphabet), to(to) { }

    State::State(int n, int token) : n(n), token(token) { }

    std::shared_ptr<Transition> State::TransitionTo(int iAlphabet, State *to)
    {
        std::shared_ptr<Transition> t = std::make_shared<Transition>(iAlphabet, to);
        transitions.push_back(t);
        return t;
    }

    void Partial::Chain(State *s)
    {
        // If we try to chain something that has no dangling pointers, we'll end
        // up with an orphaned state and leak memory. Assert to ensure this
        // doesn't happen.
        assert(dangling.size() > 0);

        for (std::shared_ptr<Transition> d : dangling) d->to = s;
    }

    Nfa::Nfa(const std::vector<std::unique_ptr<regex::Node>> &exprs) : size(0)
    {
        // First, construct a global alphabet common across all expressions
        Alphabet::Buffer buf;

        for (const std::unique_ptr<regex::Node> &expr : exprs)
        {
            extractAlphabet(*expr, buf);
        }

        Alphabet alpha(buf);
        head = newState();

        // Build an NFA for each expression, then connect each NFA to the entry
        // state with epsilon transitions.
        for (int i = 0; i < exprs.size(); i++)
        {
            Partial nfa = build(*exprs[i], alpha);

            // Connect dangling pointers to an accepting state whose token is
            // the index of the expression. We can use the index to identify the
            // token later.
            nfa.Chain(newState(i));
            head->TransitionTo(constants::epsilon, nfa.head);
        }
    }

    Nfa::~Nfa()
    {
        free(head);
    }

    State *Nfa::newState()
    {
        return newState(-1);
    }

    State *Nfa::newState(int token)
    {
        return new State(size++, token);
    }

    Partial Nfa::build(const regex::Node &expr, const Alphabet &alphabet)
    {
        switch (expr.kind)
        {
            case regex::Node::Union:
            {
                // Build NFAs from expressions on either side of the union.
                // Then, create a new state with epsilon transitions pointing
                // to both. The resulting NFA points to the new state and
                // combines the dangling pointers from both expressions.
                //
                //    ┌─ε──> L ──>
                // S ─┤
                //    └─ε──> R ──>

                State *s = newState();
                Partial left = build(*expr.left, alphabet);
                Partial right = build(*expr.right, alphabet);

                s->TransitionTo(constants::epsilon, left.head);
                s->TransitionTo(constants::epsilon, right.head);

                // We can reuse left.dangling since we no longer need it
                left.dangling.insert(left.dangling.end(), right.dangling.begin(), right.dangling.end());

                return Partial{ .head = s, .dangling = left.dangling };
            }
            case regex::Node::Concat:
            {
                // Build NFAs from expressions on either side of the
                // concatenation. Set dangling pointers from the left NFA to the
                // head of the right NFA. The resulting NFA points to the left
                // state and assumes the dangling pointers from the right.
                //
                // L ──> R ──>

                Partial left = build(*expr.left, alphabet);
                Partial right = build(*expr.right, alphabet);
                left.Chain(right.head);

                return Partial{ .head = left.head, .dangling = right.dangling };
            }
            case regex::Node::Maybe:
            {
                // Build NFA from the maybe'ed expression. Create a new state
                // that has a dangling epsilon transition and an epsilon
                // transition to the NFA. Use the same dangling pointers from
                // the maybe'ed NFA, but add the new epsilon transition as well.
                //
                //    ┌─ε──> L ──>
                // S ─┤
                //    └─ε────────>

                State *s = newState();
                Partial left = build(*expr.left, alphabet);

                s->TransitionTo(constants::epsilon, left.head);

                // We can reuse left.dangling since we no longer need it
                left.dangling.push_back(s->TransitionTo(constants::epsilon, nullptr));

                return Partial{ .head = s, .dangling = left.dangling };
            }
            case regex::Node::ZeroPlus:
            {
                // Build NFA from expression. Create a new state, add an epsilon
                // transition to the NFA, and connect the NFA's dangling
                // pointers to the new state. Add a single dangling epsilon
                // transition to the new state to satisfy the "zero" condition.
                //
                //    ┌─ε──────> L
                // S ─┤<───────ε─┘
                //    └─ε────────>

                State *s = newState();
                Partial left = build(*expr.left, alphabet);

                left.Chain(s);
                s->TransitionTo(constants::epsilon, left.head);
                std::shared_ptr<Transition> dangle = s->TransitionTo(constants::epsilon, nullptr);

                return Partial{ .head = s, .dangling = std::vector<std::shared_ptr<Transition>>{dangle} };
            }
            case regex::Node::OnePlus:
            {
                // Build NFA from expression. Create a new state with a dangling
                // epsilon transition, but position it after the NFA to require
                // the pattern at least once.
                //
                // ┌─ε─────> S ──>
                // L <─────ε─┘

                State *s = newState();
                Partial left = build(*expr.left, alphabet);

                left.Chain(s);
                s->TransitionTo(constants::epsilon, left.head);
                std::shared_ptr<Transition> dangle = s->TransitionTo(constants::epsilon, nullptr);

                return Partial{ .head = left.head, .dangling = std::vector<std::shared_ptr<Transition>>{dangle} };
            }
            case regex::Node::Range:
            {
                // Map left and right to alphabet and add one dangling
                // transition for each character range

                State *s = newState();
                Alphabet::RangeIndex idx = alphabet.Map(expr.left->g, expr.right->g);

                for (int i = idx.i; i < idx.len; i++)
                {
                    s->TransitionTo(i, nullptr);
                }

                return Partial{ .head = s, .dangling = s->transitions };
            }
            case regex::Node::Literal:
            {
                // Find character range in alphabet representing a single glyph
                // and add one transition

                State *s = newState();
                s->TransitionTo(alphabet.Map(expr.g, expr.g).i, nullptr);

                return Partial{ .head = s, .dangling = s->transitions };
            }
            case regex::Node::Wildcard:
            {
                // Add a transition for every character range in the alphabet as
                // well as a transition representing every character not in the
                // alphabet

                State *s = newState();
                s->TransitionTo(constants::negative, nullptr);

                for (int i = 0; i < alphabet.Length(); i++)
                {
                    s->TransitionTo(i, nullptr);
                }

                return Partial{ .head = s, .dangling = s->transitions };
            }
            default:
                throw std::logic_error("Unrecognized regex node kind");
        }
    }

    void Nfa::free(State *s)
    {
        std::stack<State *> stack;
        std::set<State *> visited;

        stack.push(s);

        while (!stack.empty())
        {
            s = stack.top();
            stack.pop();

            for (const std::shared_ptr<Transition> t : s->transitions)
            {
                if (t->to == nullptr || visited.count(t->to)) continue;

                visited.insert(t->to);
                stack.push(t->to);
            }

            visited.insert(s);
            delete s;
        }
    }

    int Nfa::Size() const
    {
        return size;
    }

    const State *Nfa::GetHead() const
    {
        return head;
    }
}
