#include <memory>
#include "regex.hpp"
#include "utf8.hpp"

namespace slim::regex
{
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

    Node::Node(Kind kind, std::unique_ptr<Node> left, std::unique_ptr<Node> right)
            : kind(kind)
            , left(std::move(left))
            , right(std::move(right))
            {}

    Node::Node(Kind kind, utf8::Glyph g) : kind(kind), g(g) {}

    std::unique_ptr<Node> makeUnion(std::unique_ptr<Node> left, std::unique_ptr<Node> right)
    {
        return std::make_unique<Node>(Node::Union, std::move(left), std::move(right));
    }

    std::unique_ptr<Node> makeConcat(std::unique_ptr<Node> left, std::unique_ptr<Node> right)
    {
        return std::make_unique<Node>(Node::Concat, std::move(left), std::move(right));
    }

    std::unique_ptr<Node> makeMaybe(std::unique_ptr<Node> left)
    {
        return std::make_unique<Node>(Node::Maybe, std::move(left), nullptr);
    }

    std::unique_ptr<Node> makeZeroPlus(std::unique_ptr<Node> left)
    {
        return std::make_unique<Node>(Node::ZeroPlus, std::move(left), nullptr);
    }

    std::unique_ptr<Node> makeOnePlus(std::unique_ptr<Node> left)
    {
        return std::make_unique<Node>(Node::OnePlus, std::move(left), nullptr);
    }

    std::unique_ptr<Node> makeLit(utf8::Glyph g)
    {
        return std::make_unique<Node>(Node::Literal, g);
    }

    std::unique_ptr<Node> makeWild()
    {
        return std::make_unique<Node>(Node::Wildcard, 0);
    }

    static std::unique_ptr<Node> rExpr(TokenStream &tokens);
    static std::unique_ptr<Node> rUnion(TokenStream &tokens);
    static std::unique_ptr<Node> rConcat(TokenStream &tokens);
    static std::unique_ptr<Node> rQuant(TokenStream &tokens);
    static std::unique_ptr<Node> rAtom(TokenStream &tokens);
    static std::unique_ptr<Node> rGroup(TokenStream &tokens);
    static std::unique_ptr<Node> rLit(TokenStream &tokens);

    // The entry point of a regular expression
    static std::unique_ptr<Node> rExpr(TokenStream &tokens)
    {
        if (!tokens.Advance())
        {
            tokens.Throw("Empty pattern");
        }

        std::unique_ptr<Node> tree = rUnion(tokens);
        if (tokens.Current() != EOF)
        {
            // This can happen if a close character, such as ')', appears at the
            // end of the pattern with no corresponding open character
            tokens.Throw("Unexpected character");
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
        while (tokens.Current() == '|')
        {
            if (!tokens.Advance())
            {
                throw std::runtime_error("Unexpected end of input, malformed union");
            }
            tree = makeUnion(std::move(tree), rConcat(tokens));
        }
        return tree;
    }

    // There is no metacharacter to represent concatenation. Instead, we create
    // a concat node in the parse tree whenever we see consecutive (maybe
    // quantified) atoms. Always returns a tree or throws.
    static std::unique_ptr<Node> rConcat(TokenStream &tokens)
    {
        std::unique_ptr<Node> tree = rQuant(tokens);
        if (!tree)
        {
            throw std::runtime_error("Unexpected character, expected group, character literal, or wildcard");
        }

        // Attempt to greedily exhaust all characters. If only a single atom is
        // detected, no concatenation node will be added to the parse tree.
        while (tokens.Current() != EOF)
        {
            std::unique_ptr<Node> right = rQuant(tokens);

            // If we can't match a quantified atom, concatenation is complete
            if (!right) {
                return tree;
            }

            tree = makeConcat(std::move(tree), std::move(right));
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

        // Then, look for a quantifier metacharacter. If we find one, add a node
        // to the parse tree and consume the token
        if (tokens.Advance())
        {
            switch (tokens.Current())
            {
                case '?':
                    tree = makeMaybe(std::move(tree));
                    tokens.Advance();
                    break;
                case '*':
                    tree = makeZeroPlus(std::move(tree));
                    tokens.Advance();
                    break;
                case '+':
                    tree = makeOnePlus(std::move(tree));
                    tokens.Advance();
                    break;
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
            // TODO: Support char classes
            default:
                return rLit(tokens);
        }
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
                tokens.Throw("Unexpected quantifier operator");
            case '\\':
                if (!tokens.Advance())
                {
                    tokens.Throw("Unterminated escape sequence");
                }

                // TODO: properly handle escapes
                return std::make_unique<Node>(Node::Literal, tokens.Current());
            default:
                return std::make_unique<Node>(Node::Literal, tokens.Current());
        }
    }

    std::unique_ptr<Node> Parse(std::string re) {
        TokenStream tokens(re);
        return rExpr(tokens);
    }
}
