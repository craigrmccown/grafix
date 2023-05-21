#pragma once

#include <cassert>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include "utf8.hpp"

/**
 * abc[d-f]+
 * abcd
 */
namespace slim::regex
{
    struct Node {
        enum Kind {
            Invalid,
            Char,
            Wildcard,
            Range,
            Concat,
            Union,
            Maybe,
            ZeroPlus,
            OnePlus,
        };

        Kind kind;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        utf8::Glyph g;

        // Used for binary operators, such as concatenation and union
        Node(Kind kind, std::unique_ptr<Node> left, std::unique_ptr<Node> right)
            : kind(kind)
            , left(std::move(left))
            , right(std::move(right))
            {}

        // Used for unary operators, such as quantification
        Node(Kind kind, std::unique_ptr<Node> left) : Node(kind, std::move(left), nullptr) {}

        // Used for literals
        Node(Kind kind, utf8::Glyph g) : kind(kind), g(g) {}
    };

    using Utf8StrDecoder = utf8::Decoder<std::string::iterator>;

    struct ParseState {
        // TODO: error message char number
        utf8::Glyph curr;
    };

    class TokenStream
    {
        public:
        TokenStream(std::string tokens);

        utf8::Glyph Current() const;

        bool Advance();

        void Throw(std::string message);

        private:
        utf8::Decoder<std::string::iterator> decoder;
        utf8::Glyph curr;
        unsigned int consumed;
    };

    // Initial character is ASCII NUL
    TokenStream::TokenStream(std::string tokens)
        : decoder(utf8::Decoder(tokens.begin(), tokens.end()))
        , curr(0)
        , consumed(0)
        {}
    
    utf8::Glyph TokenStream::Current() const
    {
        return curr;
    }

    bool TokenStream::Advance()
    {
        utf8::Glyph next;
        if (!decoder.Next(next))
        {
            if (decoder.Err())
            {
                Throw("Invalid UTF-8");
            }
            
            curr = EOF;
            return false;
        }

        curr = next;
        consumed++;
        return true;
    }

    static std::unique_ptr<Node> rExpr(Utf8StrDecoder &decoder, ParseState &state);
    static std::unique_ptr<Node> rGroup(Utf8StrDecoder &decoder, ParseState &state);
    static std::unique_ptr<Node> rUnion(Utf8StrDecoder &decoder, ParseState &state);
    static std::unique_ptr<Node> rConcat(Utf8StrDecoder &decoder, ParseState &state);
    static std::unique_ptr<Node> rQuant(Utf8StrDecoder &decoder, ParseState &state);
    static std::unique_ptr<Node> rAtom(Utf8StrDecoder &decoder, ParseState &state);
    static std::unique_ptr<Node> rLit(Utf8StrDecoder &decoder, ParseState &state);

    // The entry point of a regular expression. Either starts with a group or a
    // union.
    static std::unique_ptr<Node> rExpr(Utf8StrDecoder &decoder, ParseState &state)
    {
        if (decoder.Next(state.curr))
        {
            // If the first character is '(', start a new group. Otherwise,
            // interpret as a union.
            switch (state.curr)
            {
                case '(':
                    return rGroup(decoder, state);
                default:
                    return rUnion(decoder, state);
            }
        }
        if (decoder.Err())
        {
            // error: invalid utf8
        }

        throw std::runtime_error("Empty pattern");
    }

    // An explicit group, surrounded by parenthesis. Affects the shape of the
    // resulting parse tree by composing other node types, but does not create
    // nodes itself.
    static std::unique_ptr<Node> rGroup(Utf8StrDecoder &decoder, ParseState &state)
    {
        // In order to transition into a group in the first place, assume the
        // caller already matched '('. Assert for debugging purposes.
        assert(state.curr == '(');

        // Advance past the open parenthesis.
        // TODO: function to keep track of position, advance, and check decoder
        // error state
        if (!decoder.Next(state.curr))
        {
            throw std::runtime_error("Unexpected end of input, unclosed group");
        }

        // The top-level construct of a group is a union, even if the union set
        // has only one element
        std::unique_ptr<Node> tree = rUnion(decoder, state);

        // Ensure parentheses are balanced.
        if (state.curr != ')')
        {
            throw std::runtime_error("Unclosed group");
        }

        // Disallow empty groups
        if (!tree)
        {
            throw std::runtime_error("Groups cannot be empty");
        }

        return tree;
    }

    // One or more concatenations, separated by pipes. A single concatenation is
    // logically treated as a union set with one element.
    static std::unique_ptr<Node> rUnion(Utf8StrDecoder &decoder, ParseState &state)
    {
        std::unique_ptr<Node> tree = rConcat(decoder, state);

        // If the current character is a pipe, create a union node in the parse
        // tree and advance the decoder. If there are no pipes, no extraneous
        // union nodes will be created.
        while (state.curr == '|' && decoder.Next(state.curr))
        {
            tree = std::make_unique<Node>(Node::Union, std::move(tree), rConcat(decoder, state));
        }
        return tree;
    }

    // There is no metacharacter to represent concatenation. Instead, we create
    // a concat node in the parse tree whenever we see consecutive (maybe
    // quantified) atoms.
    static std::unique_ptr<Node> rConcat(Utf8StrDecoder &decoder, ParseState &state)
    {
        std::unique_ptr<Node> tree = rQuant(decoder, state);

        // Attempt to greedily exhaust all characters. If only a single atom is
        // detected, no concatenation node will be added to the parse tree.
        while (!decoder.Done())
        {
            std::unique_ptr<Node> right = rQuant(decoder, state);

            // If we can't match a quantified atom, concatenation is complete
            if (!right) {
                return tree;
            }

            tree = std::make_unique<Node>(Node::Concat, std::move(tree), std::move(right));
        }
        return tree;
    }
 
    // Parses an atom with an optional quantifier. Returns nullptr if an atom
    // can't be matched.
    static std::unique_ptr<Node> rQuant(Utf8StrDecoder &decoder, ParseState &state)
    {
        // First, try to parse an atom
        std::unique_ptr<Node> tree = rAtom(decoder, state);
        if (!tree)
        {
            return tree;
        }

        // Then, look for a quantifier metacharacter
        if (decoder.Next(state.curr))
        {
            Node::Kind kind = Node::Invalid;
            switch (state.curr)
            {
                case '?':
                    kind = Node::Maybe;
                    break;
                case '*':
                    kind = Node::ZeroPlus;
                    break;
                case '+':
                    kind = Node::OnePlus;
                    break;
            }

            // If we find a quantifier, add a node to the parse tree and consume
            // the token
            if (kind != Node::Invalid)
            {
                tree = std::make_unique<Node>(kind, std::move(tree));
                decoder.Next(state.curr);
            }
        }

        return tree;
    }

    // An atom is any singular expression that can be quantified, such as a
    // character literal, character class, wildcard, or group.
    static std::unique_ptr<Node> rAtom(Utf8StrDecoder &decoder, ParseState &state)
    {
        std::unique_ptr<Node> tree;

        // If we receive a close parenthesis or a pipe, refuse to parse an atom.
        // Returning nullptr signals to the caller that it should return control
        // to a higher-level parse state.
        switch (state.curr)
        {
            case ')':
            case '|':
                return nullptr;
            case '(':
                return rGroup(decoder, state);
            case '.':
                return std::make_unique<Node>(Node::Wildcard, 0);
            default:
                return rLit(decoder, state);
        }
    }

    // Parses a character literal that is potentially escaped.
    static std::unique_ptr<Node> rLit(Utf8StrDecoder &decoder, ParseState &state)
    {
        // Assume the caller has already checked for certain metacharacters.
        // Assert for debugging purposes.
        assert(state.curr != '(' && state.curr != ')' && state.curr != '.' && state.curr != '|');

        switch (state.curr)
        {
            case '?':
            case '*':
            case '+':
                throw std::runtime_error("Unexpected metacharacter");
            case '\\':
                if (!decoder.Next(state.curr))
                {
                    throw std::runtime_error("Unterminated escape sequence");
                }

                // TODO: properly handle escapes
                return std::make_unique<Node>(Node::Char, state.curr);
            default:
                return std::make_unique<Node>(Node::Char, state.curr);
        }
    }

    std::unique_ptr<Node> parse(std::string re) {
        utf8::Decoder decoder(re.begin(), re.end());
        ParseState state { .curr = 0 };

        return rExpr(decoder, state);
    }
}
