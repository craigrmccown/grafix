#pragma once

#include <utility>
#include <vector>
#include "utf8.hpp"

namespace slim
{
    // Holds glyph pairs representing ranges found in an expression's
    // alphabet, sorted. Each range is treated as a single character in the
    // context of state machine transitions. All ranges are closed on the
    // start and end and disjoint. The ranges in an alphabet are
    // non-overlapping, and every range in the original expression can be
    // mapped to one or more consecutive ranges in the alphabet.
    //
    // For example, an expression containing ranges 1-3, 6-8, 1-9 would
    // produce this alphabet: 1-3, 4-5, 6-8, 9-9
    class Alphabet {
        public:
        // Represents an endpoint of a range. The second element signals
        // whether the endpoint is the start or end of the range.
        using Bound = std::pair<utf8::Glyph, bool>;
        using Range = std::pair<utf8::Glyph, utf8::Glyph>;

        struct RangeIndex {
            int i;
            int len;
        };

        // Used to incrementally collect glyph ranges before constructing an
        // alphabet
        class Buffer
        {
            public:
            void Write(utf8::Glyph g);
            void Write(utf8::Glyph start, utf8::Glyph end);
            const std::vector<Bound> &Get() const;

            private:
            std::vector<Bound> bounds;
        };

        Alphabet(const Buffer &buf);

        // Returns the index of the starting alphabet range corresponding to
        // the start glyph and the number of consecutive indices until the
        // end
        RangeIndex Map(utf8::Glyph start, utf8::Glyph end) const;

        // Returns the index of the range that contains the given glyph or -1 if
        // it does not fall within a range
        int IndexOf(utf8::Glyph g) const;

        // Returns the number of ranges in the alphabet
        int Length() const;

        private:
        // An ordered set of glyph ranges, where no two ranges overlap and
        // each range is closed on its endpoints (inclusive). For example,
        // a-c includes the characters a, b, and c. Each even index is the
        // start of a range, and each odd index is the end of a range.
        std::vector<utf8::Glyph> ranges;
    };
}
