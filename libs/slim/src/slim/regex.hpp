#pragma once

#include <cassert>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include "utf8.hpp"

namespace slim::regex
{
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
 
    struct Node {
        enum Kind {
            Literal,
            Wildcard,
            Range,
            Concat,
            Union,
            Maybe,
            ZeroPlus,
            OnePlus,
        };

        Kind kind;

        // Always has a value for operator kinds. Used as the left operand for
        // binary operators or the only operand for unary operators.
        std::unique_ptr<Node> left;

        // Only has a value for binary operators, such as concatenation and
        // union
        std::unique_ptr<Node> right;

        // Only contains a meaningful value if the kind is literal
        utf8::Glyph g;

        // Used for operators, such as concatenation and union
        Node(Kind kind, std::unique_ptr<Node> left, std::unique_ptr<Node> right);

        // Used for literals
        Node(Kind kind, utf8::Glyph g);
    };

    std::unique_ptr<Node> makeUnion(std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    std::unique_ptr<Node> makeConcat(std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    std::unique_ptr<Node> makeMaybe(std::unique_ptr<Node> left);
    std::unique_ptr<Node> makeZeroPlus(std::unique_ptr<Node> left);
    std::unique_ptr<Node> makeOnePlus(std::unique_ptr<Node> left);
    std::unique_ptr<Node> makeRange(std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    std::unique_ptr<Node> makeLit(utf8::Glyph g);
    std::unique_ptr<Node> makeWild();

    std::unique_ptr<Node> Parse(std::string re);
}
