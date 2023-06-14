#include <cstdint>
#include <queue>
#include <utility>
#include <vector>
#include "automaton.hpp"
#include "utf8.hpp"

namespace slim
{
    namespace nfa
    {
        void Alphabet::Buffer::Write(utf8::Glyph g)
        {
            Write(g, g);
        }

        void Alphabet::Buffer::Write(utf8::Glyph start, utf8::Glyph end)
        {
            // If a range will never match, we can safely ignore it
            if (start > end) return;

            bounds.emplace_back(start, true);
            bounds.emplace_back(end, false);
        }

        const std::vector<Alphabet::Bound> &Alphabet::Buffer::Get() const
        {
            return bounds;
        }

        // Returns a key that can be used to compare two bounds. The most
        // significant bits are the glyph, and the least significant bit is
        // whether the bound is at the start or end of the range as a tie
        // breaker.
        static u_int64_t getBoundSortKey(Alphabet::Bound b)
        {
            return ((u_int64_t)b.first << 1) | !b.second;
        }

        // Construct consecutive, non-overlapping ranges from the unordered,
        // arbitrary ranges contained in the buffer. Runs in O(n log(n)).
        Alphabet::Alphabet(const Buffer &buf)
        {
            // Start by constructing a priority queue of bounds. Underlying heap
            // can be constructed in linear time via Floyd's.
            const std::vector<Bound> bounds = buf.Get();
            std::priority_queue<Bound, std::vector<Bound>, std::function<bool(Bound, Bound)>> pq(
                bounds.begin(),
                bounds.end(),
                // We will receive elements in ascending order. For bounds with
                // duplicate glyphs, we first receive starts, then ends.
                [](Bound a, Bound b) { return getBoundSortKey(a) > getBoundSortKey(b); }
            );

            // We use this counter to determine whether we are within an
            // overlapping range as we consume the queue.
            int overlap = 0;

            // This is used to skip duplicate entries. For example, if two
            // ranges start on the same glyph, we only care about the first one.
            Bound prev;

            while (!pq.empty())
            {
                const Bound curr = pq.top();
                pq.pop();

                utf8::Glyph g = curr.first;
                bool start = curr.second;

                if (overlap == 0)
                {
                    // If the input is well-formed, we should always start a new
                    // range here
                    assert(start);

                    // We aren't in an overlapping range, so it's okay if we
                    // leave a gap between the last and current ranges. Only add
                    // the current glyph.
                    overlap++;
                    ranges.push_back(g);
                    prev = curr;
                    continue;
                }

                // Keep track of the number of ranges that have been started
                // but not ended. When all ranges that have started have also
                // been ended, overlap will be 0.
                if (start) overlap++;
                else overlap--;

                // If two ranges start or end on the same glyph, we ignore them.
                // Otherwise, we will generate overlapping ranges.
                if (prev.first == g && prev.second == start) continue;
                prev = curr;

                // Whether or not the last range in the output has both a start
                // and end element. By definition, all ranges in the output
                // other than the last range are complete.
                bool complete = !(ranges.size() % 2);
                utf8::Glyph last = ranges[ranges.size() - 1];

                if (start)
                {
                    if (complete)
                    {
                        // A larger range overlaps this one. If there is a gap
                        // between the last and current ranges, fill it with a
                        // new range.
                        if (last + 1 != g)
                        {
                            ranges.push_back(last + 1);
                            ranges.push_back(g - 1);
                        }
                    }
                    else
                    {
                        // Close the last range before starting the new one. The
                        // current glyph must be greater than the start of the
                        // last range because of the sort order.
                        ranges.push_back(g - 1);
                    }

                    // Start a new range
                    ranges.push_back(g);
                }
                else
                {
                    // If there's no unclosed range, start a new range and
                    // immediately end it. Otherwise, simply end the unclosed
                    // range.
                    if (complete) ranges.push_back(last + 1);
                    ranges.push_back(g);
                }
            }
        }

        Alphabet::RangeIndex Alphabet::Map(utf8::Glyph start, utf8::Glyph end) const
        {
            // We could perform a binary search instead, but a linear search is
            // sufficient for small alphabets
            for (int i = 0; i < ranges.size(); i += 2)
            {
                if (ranges[i] == start)
                {
                    int startIdx = i;

                    for (i++; i < ranges.size(); i += 2)
                    {
                        if (ranges[i] == end)
                        {
                            return RangeIndex{.i = startIdx / 2, .len = (i + 1 - startIdx) / 2};
                        }
                    }
                }
            }

            throw std::logic_error("Could not map range to alphabet");
        }

        Alphabet::Range Alphabet::Get(int i) const
        {
            i *= 2;

            if (i >= ranges.size() || i < 0)
            {
                throw std::logic_error("Index out of bounds");
            }

            return Range(ranges[i], ranges[i + 1]);
        }

        int Alphabet::Length() const
        {
            return ranges.size() / 2;
        }
    }
}
