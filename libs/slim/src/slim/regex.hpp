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

    // Decodes each code point of a UTF-8 encoded string, tracking minimal state
    // as each token is consumed
    class TokenStream
    {
        public:
        TokenStream(const std::string &tokens);

        // Returns the current glyph. If the entire stream has been consumed,
        // returns EOF.
        utf8::Glyph Current() const;

        // Advances the stream by a single glyph. Throws if invalid UTF-8 is
        // detected.
        bool Advance();

        // Throws an exception containing helpful debugging context
        void Throw(const std::string &message) const;

        private:
        utf8::Decoder<std::string::const_iterator> decoder;
        utf8::Glyph curr;
        unsigned int consumed;
    };

    // Initial character is ASCII NUL
    TokenStream::TokenStream(const std::string &tokens)
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

    void TokenStream::Throw(const std::string &message) const
    {
        throw std::runtime_error("Error at position " + std::to_string(consumed) + ": " + message);
    }

    static std::unique_ptr<Node> rExpr(TokenStream &tokens);
    static std::unique_ptr<Node> rGroup(TokenStream &tokens);
    static std::unique_ptr<Node> rUnion(TokenStream &tokens);
    static std::unique_ptr<Node> rConcat(TokenStream &tokens);
    static std::unique_ptr<Node> rQuant(TokenStream &tokens);
    static std::unique_ptr<Node> rAtom(TokenStream &tokens);
    static std::unique_ptr<Node> rLit(TokenStream &tokens);

    // The entry point of a regular expression. Either starts with a group or a
    // union.
    static std::unique_ptr<Node> rExpr(TokenStream &tokens)
    {
        if (tokens.Advance())
        {
            // If the first character is '(', start a new group. Otherwise,
            // interpret as a union.
            switch (tokens.Current())
            {
                case '(':
                    return rGroup(tokens);
                default:
                    return rUnion(tokens);
            }
        }

        tokens.Throw("Empty pattern");
    }

    // An explicit group, surrounded by parenthesis. Affects the shape of the
    // resulting parse tree by composing other node types, but does not create
    // nodes itself.
    static std::unique_ptr<Node> rGroup(TokenStream &tokens)
    {
        // In order to transition into a group in the first place, assume the
        // caller already matched '('. Assert for debugging purposes.
        assert(tokens.Current() == '(');

        // Advance past the open parenthesis.
        // TODO: function to keep track of position, advance, and check decoder
        // error state
        if (!tokens.Advance())
        {
            tokens.Throw("Unexpected end of input, unclosed group");
        }

        // The top-level construct of a group is a union, even if the union set
        // has only one element
        std::unique_ptr<Node> tree = rUnion(tokens);

        // Ensure parentheses are balanced.
        if (tokens.Current() != ')')
        {
            tokens.Throw("Unclosed group");
        }

        // Disallow empty groups
        if (!tree)
        {
            tokens.Throw("Groups cannot be empty");
        }

        return tree;
    }

    // One or more concatenations, separated by pipes. A single concatenation is
    // logically treated as a union set with one element.
    static std::unique_ptr<Node> rUnion(TokenStream &tokens)
    {
        std::unique_ptr<Node> tree = rConcat(tokens);

        // If the current character is a pipe, create a union node in the parse
        // tree and advance the decoder. If there are no pipes, no extraneous
        // union nodes will be created.
        while (tokens.Current() == '|' && tokens.Advance())
        {
            tree = std::make_unique<Node>(Node::Union, std::move(tree), rConcat(tokens));
        }
        return tree;
    }

    // There is no metacharacter to represent concatenation. Instead, we create
    // a concat node in the parse tree whenever we see consecutive (maybe
    // quantified) atoms.
    static std::unique_ptr<Node> rConcat(TokenStream &tokens)
    {
        std::unique_ptr<Node> tree = rQuant(tokens);

        // Attempt to greedily exhaust all characters. If only a single atom is
        // detected, no concatenation node will be added to the parse tree.
        while (tokens.Current() != EOF)
        {
            std::unique_ptr<Node> right = rQuant(tokens);

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
    static std::unique_ptr<Node> rQuant(TokenStream &tokens)
    {
        // First, try to parse an atom
        std::unique_ptr<Node> tree = rAtom(tokens);
        if (!tree)
        {
            return tree;
        }

        // Then, look for a quantifier metacharacter
        if (tokens.Advance())
        {
            Node::Kind kind = Node::Invalid;
            switch (tokens.Current())
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
                tokens.Advance();
            }
        }

        return tree;
    }

    // An atom is any singular expression that can be quantified, such as a
    // character literal, character class, wildcard, or group.
    static std::unique_ptr<Node> rAtom(TokenStream &tokens)
    {
        std::unique_ptr<Node> tree;

        // If we receive a close parenthesis or a pipe, refuse to parse an atom.
        // Returning nullptr signals to the caller that it should return control
        // to a higher-level parse state.
        switch (tokens.Current())
        {
            case ')':
            case '|':
                return nullptr;
            case '(':
                return rGroup(tokens);
            case '.':
                return std::make_unique<Node>(Node::Wildcard, 0);
            default:
                return rLit(tokens);
        }
    }

    // Parses a character literal that is potentially escaped.
    static std::unique_ptr<Node> rLit(TokenStream &tokens)
    {
        // Assume the caller has already checked for certain metacharacters.
        // Assert for debugging purposes.
        assert(tokens.Current() != '(' && tokens.Current() != ')' && tokens.Current() != '.' && tokens.Current() != '|');

        switch (tokens.Current())
        {
            case '?':
            case '*':
            case '+':
                tokens.Throw("Unexpected metacharacter");
            case '\\':
                if (!tokens.Advance())
                {
                    tokens.Throw("Unterminated escape sequence");
                }

                // TODO: properly handle escapes
                return std::make_unique<Node>(Node::Char, tokens.Current());
            default:
                return std::make_unique<Node>(Node::Char, tokens.Current());
        }
    }

    std::unique_ptr<Node> parse(std::string re) {
        TokenStream tokens(re);
        return rExpr(tokens);
    }
}
