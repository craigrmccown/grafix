#pragma once

#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include "dfa.hpp"
#include "nfa.hpp"
#include "regex.hpp"
#include "utf8.hpp"

// A guess at the longest typical token length. Used to pre-allocate memory and
// avoid re-allocating the token buffer. Expressed in units of
// sizeof(utf8::Glyph), or 4 bytes.
const int defaultTokenBufferSize = 20;

// Restrict token buffer size to 4KiB, plus overhead
const int maxTokenBufferSize = 1024;

namespace slim
{
    // TODO: Move definition to source file
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
            case regex::Node::Wildcard:
                break;
            case regex::Node::Concat:
            case regex::Node::Union:
                extractAlphabet(*expr.left, buf);
                extractAlphabet(*expr.right, buf);
                break;
            case regex::Node::Maybe:
            case regex::Node::ZeroPlus:
            case regex::Node::OnePlus:
                extractAlphabet(*expr.left, buf);
                break;
        }
    }

    static inline bool isNewline(utf8::Glyph g)
    {
        return g == '\n';
    }

    static inline bool isWhitespace(utf8::Glyph g)
    {
        return g == ' ' || g == '\t';
    }

    struct Token
    {
        int i, line, col;
        std::vector<utf8::Glyph> data;

        // TODO: Move this logic to utf8.hpp
        std::string ToString() const
        {
            std::stringstream ss;

            for (utf8::Glyph g : data)
            {
                int i;
                char c;

                for (i = 0; i < 3; i++)
                {
                    c = g >> ((3 - i) * 8);
                    if (c != 0) {
                        break;
                    }
                }

                for (; i < 4; i++)
                {
                    c = g >> ((3 - i) * 8);
                    ss << c;
                }
            }

            return ss.str();
        }

        // Throws an error with a standard message format, including the token's
        // line and column
        void Throw(const std::string &message) const
        {
            std::stringstream ss;
            ss
                << "Error at " << line << ":" << col << ": '"
                << ToString() << "'\n"
                << "  " << message;
            throw std::runtime_error(ss.str());
        }
    };

    // Abstracts the lexer implementation. Allows the parser to consumer tokens
    // from any source without relying on an underlying implementation.
    //
    // TODO: Produce a sentinel EOF token when input ends instead of returning
    // boolean
    class TokenIter
    {
        public:
        virtual bool Next(Token &token) = 0;
    };

    template <typename ByteIterator>
    class Lexer : public TokenIter
    {
        public:
        Lexer(
            const std::vector<std::string> &patterns,
            ByteIterator begin,
            ByteIterator end
        )
            : shouldAdvance(true)
            // Initialize to -1 because nothing has been consumed from the
            // input. The first glyph consumed should be at col == 0.
            , col(-1)
            , line(1)
            , input(begin, end)
        {
            std::vector<std::unique_ptr<regex::Node>> exprs;
            exprs.reserve(patterns.size());

            for (const std::string &p : patterns)
            {
                exprs.push_back(regex::Parse(p));
            }

            // Construct a global alphabet common across all expressions
            Alphabet::Buffer buf;
            for (const std::unique_ptr<regex::Node> &expr : exprs)
            {
                extractAlphabet(*expr, buf);
            }

            alpha = std::make_unique<Alphabet>(buf);
            machine = std::make_unique<dfa::Dfa>(nfa::Nfa(*alpha, exprs));
            tokBuf.reserve(defaultTokenBufferSize);
        }

        // Produces the next token and returns true if the input has not been
        // consumed. Throws an error if an invalid token is encountered, but
        // will continue attempting to consume the input and produce valid
        // tokens on subseqent calls.
        bool Next(Token &token)
        {
            dfa::State *curr = machine->GetHead();

            // Respect shouldAdvance by short circuiting here
            while (!shouldAdvance || next())
            {
                shouldAdvance = true;
                bool newline = isNewline(g);
                dfa::State *next;

                // Always discard newlines and assume end of token, disallowing
                // tokens that contain newlines, even if a pattern would accept
                // them. For all other cases, attempt to transition to the next
                // state.
                if (newline || !(next = curr->GoTo(alpha->IndexOf(g))))
                {
                    // For convenience, newlines and whitespace that come before
                    // a token are discarded so that whitespace can be used to
                    // separate tokens without needing special parser rules.
                    // Greedily consume and skip newlines and whitespace as long
                    // as no input has been matched.
                    if (tokBuf.size() == 0 && (newline || isWhitespace(g))) continue;

                    // Only attempt to produce a token when we fail to match the
                    // input. This policy produces the longest matching
                    // sequence.
                    if (curr->token >= 0)
                    {
                        // Newlines should always be eaten
                        shouldAdvance = newline;
                        produceToken(token, curr->token);
                        return true;
                    }
                    else
                    {
                        fail("Unexpected character");
                    }
                }

                // Set a hard limit on the size for a single token
                if (tokBuf.size() > maxTokenBufferSize)
                {
                    fail(
                        "Token exceeds maximum length of " +
                        std::to_string(maxTokenBufferSize) +
                        " characters"
                    );
                }

                tokBuf.push_back(g);
                curr = next;
            }

            if (input.Err())
            {
                fail("Invalid UTF-8");
            }

            // Produce the final token after input has been consumed
            if (tokBuf.size())
            {
                if (curr->token >= 0)
                {
                    produceToken(token, curr->token);
                    return true;
                }
                else
                {
                    fail("Unexpected end of input");
                }
            }

            return false;
        }

        private:
        // Signals whether to peek the previous character or consume from the
        // input. Prevents off-by-one errors when we fail to match; the failure
        // concludes the current token, but the current character still needs to
        // be matched to the next token.
        bool shouldAdvance;
        int col, line;
        std::vector<utf8::Glyph> tokBuf;
        utf8::Decoder<ByteIterator> input;
        utf8::Glyph g;
        std::unique_ptr<Alphabet> alpha;
        std::unique_ptr<dfa::Dfa> machine;

        void fail(const std::string &message)
        {
            // Invalidate the current token on failure, but don't release the
            // underlying memory
            tokBuf.clear();
            throw std::runtime_error("Error (line " + std::to_string(line) + ", col "+ std::to_string(col) +"): " + message);
        }

        void produceToken(Token &token, int i)
        {
            // It should be impossible to produce an empty token
            assert(tokBuf.size() > 0);

            std::vector<utf8::Glyph> data(tokBuf.begin(), tokBuf.end());
            tokBuf.clear();
            token = Token{
                .i = i,
                .line = line,
                .col = col - (int)data.size(),
                .data = data
            };
        }

        bool next()
        {
            // Perform bookkeeping so that we can output line/column numbers
            // on error
            if (isNewline(g))
            {
                col = 0;
                line++;
            }
            else
            {
                col++;
            }

            return input.Next(g);
        }
    };
}
