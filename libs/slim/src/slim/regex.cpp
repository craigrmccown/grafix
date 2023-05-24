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

    std::unique_ptr<Node> makeRange(std::unique_ptr<Node> left, std::unique_ptr<Node> right)
    {
        return std::make_unique<Node>(Node::Range, std::move(left), std::move(right));
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
    static std::unique_ptr<Node> rClass(TokenStream &tokens);
    static std::unique_ptr<Node> rRange(TokenStream &tokens);
    static std::unique_ptr<Node> rClassLit(TokenStream &tokens);
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
    // quantified) atoms.
    static std::unique_ptr<Node> rConcat(TokenStream &tokens)
    {
        std::unique_ptr<Node> tree = rQuant(tokens);

        // A concatenation ends at the end of the expression, a union, or the
        // end of a group.
        while (tokens.Current() != EOF && tokens.Current() != '|' && tokens.Current() != ')')
        {
            tree = makeConcat(std::move(tree), rQuant(tokens));
        }

        return tree;
    }

    // Parses an atom with an optional quantifier
    static std::unique_ptr<Node> rQuant(TokenStream &tokens)
    {
        std::unique_ptr<Node> tree = rAtom(tokens);

        // Look for a quantifier operator. If found, add a node to the parse
        // tree and consume the token.
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

        return tree;
    }

    // An atom is any singular expression that can be quantified, such as a
    // character literal, character class, wildcard, or group.
    static std::unique_ptr<Node> rAtom(TokenStream &tokens)
    {
        switch (tokens.Current())
        {
            case '(':
                return rGroup(tokens);
            case '[':
                return rClass(tokens);
            case '.':
                // Remember to consume the wildcard token
                tokens.Advance();
                return std::make_unique<Node>(Node::Wildcard, 0);
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

        // Ensure parentheses are balanced
        if (tokens.Current() != ')')
        {
            tokens.Throw("Unclosed group");
        }

        // Consume the close parenthesis
        tokens.Advance();
        return tree;
    }

    // A character class, expressed as a union of character literals and ranges
    static std::unique_ptr<Node> rClass(TokenStream &tokens)
    {
        // The caller should have checked for an open bracket before calling
        // this function. Assert for debugging purposes.
        assert(tokens.Current() == '[');

        // Consume the open bracket
        if (!tokens.Advance())
        {
            tokens.Throw("Unclosed character class");
        }

        std::unique_ptr<Node> tree = rRange(tokens);

        // Continue to consume ranges until a closing bracket is reached
        while (tokens.Current() != ']')
        {
            tree = makeUnion(std::move(tree), rRange(tokens));
        }

        // Consume the closing bracket
        tokens.Advance();
        return tree;
    }

    // A range of characters. The order of characters in the range is not
    // validated - a range whose left character is greater than its right will
    // not match anything. Only evaluated within a character class.
    static std::unique_ptr<Node> rRange(TokenStream &tokens)
    {
        std::unique_ptr<Node> tree = rClassLit(tokens);

        // If we encounter a hyphen, consume it and add a range node to the
        // parse tree. Otherwise, return the class literal node directly.
        if (tokens.Current() == '-')
        {
            if (!tokens.Advance())
            {
                tokens.Throw("Unterminated character range");
            }

            tree = makeRange(std::move(tree), rClassLit(tokens));
        }

        return tree;
    }

    // Parses a literal within a character class, which follows slightly
    // different rules than literals outside of character classes
    static std::unique_ptr<Node> rClassLit(TokenStream &tokens)
    {
        std::unique_ptr<Node> tree;

        switch (tokens.Current())
        {
            case '[':
            case ']':
            case '-':
                tokens.Throw("Illegal character");
            case '\\':
                if (!tokens.Advance())
                {
                    tokens.Throw("Unterminated escape sequence");
                }

                switch(tokens.Current())
                {
                    case '\\':
                    case '[':
                    case ']':
                    case '-':
                        tree = std::make_unique<Node>(Node::Literal, tokens.Current());
                        break;
                    case 'n': // Newline
                        tree = std::make_unique<Node>(Node::Literal, 0xA);
                        break;
                    default:
                        tokens.Throw("Invalid escape sequence");
                }
            default:
                tree = std::make_unique<Node>(Node::Literal, tokens.Current());
        }

        // Consume the class literal token
        tokens.Advance();
        return tree;
    }

    // Parses a character literal that is potentially escaped.
    static std::unique_ptr<Node> rLit(TokenStream &tokens)
    {
        // Assume the caller has already checked for certain metacharacters.
        // Assert for debugging purposes.
        assert(
            tokens.Current() != '(' &&
            tokens.Current() != '.' &&
            tokens.Current() != '['
        );

        std::unique_ptr<Node> tree;

        switch (tokens.Current())
        {
            case '?':
            case '*':
            case '+':
            case '|':
            case ')':
            case ']':
                tokens.Throw("Illegal character");
            case '\\':
                if (!tokens.Advance())
                {
                    tokens.Throw("Unterminated escape sequence");
                }

                switch(tokens.Current())
                {
                    case '\\':
                    case '?':
                    case '*':
                    case '+':
                    case '|':
                    case '(':
                    case ')':
                    case '[':
                    case ']':
                    case '.':
                        tree = std::make_unique<Node>(Node::Literal, tokens.Current());
                        break;
                    case 'n': // Newline
                        tree = std::make_unique<Node>(Node::Literal, 0xA);
                        break;
                    default:
                        tokens.Throw("Invalid escape sequence");
                }
            default:
                tree = std::make_unique<Node>(Node::Literal, tokens.Current());
        }

        // Consume the character literal token
        tokens.Advance();
        return tree;
    }

    std::unique_ptr<Node> Parse(std::string re) {
        TokenStream tokens(re);
        return rExpr(tokens);
    }
}
