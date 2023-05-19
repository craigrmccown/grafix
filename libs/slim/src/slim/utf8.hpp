#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>

namespace slim::utf8
{
    using Glyph = uint32_t;

    enum Error
    {
        None = 0,

        // The leftmost bits of the first byte that are used to specify the
        // variable length of the code point are invalid
        ErrVarlenMarkerInvalid,

        // The leftmost bits of the 2nd-4th byte of a variable-length glyph
        // are invalid
        ErrVarlenByteInvalid,

        // The input has been exhausted, but the final glyph is not complete
        ErrInputExhausted,
    };

    // Takes the first byte of a UTF-8 glyph and determines the byte length
    static uint8_t getCodePointLength(uint8_t b)
    {
        if (b < 0x80) return 1; // Leading zero (ASCII)
        if (b >> 5 == 6) return 2; // Bit pattern 110
        else if (b >> 4 == 0xE) return 3; // Bit pattern 1110
        else if (b >> 3 == 0x1E) return 4; // Bit pattern 11110
        else return 0; // Caller must check for 0 to detect invalid marker
    }

    // Validates that subsequent bytes for variable length glyphs begin with the
    // correct bit pattern
    static bool isVarlenByteValid(uint8_t c) {
        return c >> 6 == 0x2; // Bit pattern 10
    }

    template <typename ByteIterator>
    class Decoder {
        public:
        static_assert(
            std::is_convertible<typename std::iterator_traits<ByteIterator>::value_type, uint8_t>::value,
            "Wrong iterator type, must be byte"
        );

        Decoder(ByteIterator begin, ByteIterator end)
            : begin(begin)
            , end(end)
            , err(Error::None)
            {}

        bool Next(Glyph &out)
        {
            // If iterator is exhausted or there has been an error, always
            // signal completion
            if (begin == end || err) return false;

            uint8_t b = *begin;
            uint8_t len = getCodePointLength(b);
            if (len == 0) {
                err = ErrVarlenMarkerInvalid;
                return false;
            }

            Glyph next = b;
            ++begin;

            for (uint8_t i = 1; i < len; i++) {
                if (begin == end) {
                    err = ErrInputExhausted;
                    return false;
                }

                b = *begin;
                if (!isVarlenByteValid(b)) {
                    err = ErrVarlenByteInvalid;
                    return false;
                }

                // Shift up one byte and copy current into least significant
                // byte (little endian)
                next = (next << (i * 8)) | b;
                ++begin;
            }

            out = next;
            return true;
        }

        Error Err() {
            return err;
        }

        private:
        ByteIterator begin, end;
        Error err;
    };
}
