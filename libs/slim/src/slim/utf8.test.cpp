#include <string>
#include <catch2/catch_test_macros.hpp>
#include "utf8.hpp"

void testDecode(std::string input, slim::utf8::Glyph expected[])
{
    slim::utf8::Decoder decoder(input.begin(), input.end());

    int i = 0;
    slim::utf8::Glyph g;
    while (decoder.Next(g))
    {
        REQUIRE(expected[i++] == g);
    }
    REQUIRE(!decoder.Err());
}

void testDecodeErr(std::string input, slim::utf8::Error err)
{
    slim::utf8::Decoder decoder(input.begin(), input.end());

    slim::utf8::Glyph g;
    while (decoder.Next(g)) {}

    REQUIRE(decoder.Err() == err);
}

TEST_CASE("ascii as UTF8", "[slim]")
{
    std::string input = "abcde";
    slim::utf8::Glyph expected[] = {97, 98, 99, 100, 101};

    testDecode(input, expected);
}

TEST_CASE("various international characters", "[slim]")
{
    std::string input = u8"ΩѢՁېయฬ࿊ᚤ";
    slim::utf8::Glyph expected[] = {0xCEA9, 0xD1A2, 0xD581, 0xDB90, 0xE0B0AF, 0xE0B8AC, 0xE0BF8A, 0xE19AA4};

    testDecode(input, expected);
}

TEST_CASE("emojis", "[slim]")
{
    std::string input = u8"🤓🤡🤯🧢🪐🫰";
    slim::utf8::Glyph expected[] = {0xF09FA493, 0xF09FA4A1, 0xF09FA4AF, 0xF09FA7A2, 0xF09FAA90, 0xF09FABB0};

    testDecode(input, expected);
}

TEST_CASE("invalid varlen marker", "[slim]")
{
    std::string input({(char)0x80, (char)0x8F});
    testDecodeErr(input, slim::utf8::ErrVarlenMarkerInvalid);
}

TEST_CASE("invalid subsequent byte", "[slim]")
{
    std::string input({(char)0xC1, (char)0xCF});
    testDecodeErr(input, slim::utf8::ErrVarlenByteInvalid);
}

TEST_CASE("early termination", "[slim]")
{
    std::string input({(char)0xE1, (char)0x8F});
    testDecodeErr(input, slim::utf8::ErrInputExhausted);
}
