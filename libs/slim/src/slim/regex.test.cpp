#include <catch2/catch_test_macros.hpp>
#include "regex.hpp"

bool treesEq(const slim::regex::Node &a, const slim::regex::Node &b)
{
    if (a.kind != b.kind) return false;
    if (a.kind == slim::regex::Node::Literal && a.g != b.g) return false;
    if (!a.right && !b.right) return true;
    if (!a.right || !b.right) return false;

    return treesEq(*a.right, *b.right) && treesEq(*a.left, *b.left);
}

TEST_CASE("single character literal", "[slim]")
{
    std::string pattern = "a";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeLit('a');
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);
    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("multichar concat", "[slim]")
{
    std::string pattern = "ab";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeConcat(
        slim::regex::makeLit('a'),
        slim::regex::makeLit('b')
    );
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);

    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("union only", "[slim]")
{
    std::string pattern = "ab|cd|ef";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeUnion(
        slim::regex::makeUnion(
            slim::regex::makeConcat(
                slim::regex::makeLit('a'),
                slim::regex::makeLit('b')
            ),
            slim::regex::makeConcat(
                slim::regex::makeLit('c'),
                slim::regex::makeLit('d')
            )
        ),
        slim::regex::makeConcat(
            slim::regex::makeLit('e'),
            slim::regex::makeLit('f')
        )
    );
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);

    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("union grouped", "[slim]")
{
    std::string pattern = "ab|(cd|ef)";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeUnion(
        slim::regex::makeConcat(
            slim::regex::makeLit('a'),
            slim::regex::makeLit('b')
        ),
        slim::regex::makeUnion(
            slim::regex::makeConcat(
                slim::regex::makeLit('c'),
                slim::regex::makeLit('d')
            ),
            slim::regex::makeConcat(
                slim::regex::makeLit('e'),
                slim::regex::makeLit('f')
            )
        )
    );
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);

    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("concat then union", "[slim]")
{
    std::string pattern = "abc(a|b)";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeConcat(
        slim::regex::makeConcat(
            slim::regex::makeConcat(
                slim::regex::makeLit('a'),
                slim::regex::makeLit('b')
            ),
            slim::regex::makeLit('c')
        ),
        slim::regex::makeUnion(
            slim::regex::makeLit('a'),
            slim::regex::makeLit('b')
        )
    );
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);

    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("concat with quantifiers", "[slim]")
{
    std::string pattern = "a?b+";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeConcat(
        slim::regex::makeMaybe(
            slim::regex::makeLit('a')
        ),
        slim::regex::makeOnePlus(
            slim::regex::makeLit('b')
        )
    );
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);

    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("union with quantifiers", "[slim]")
{
    std::string pattern = "a?|b+";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeUnion(
        slim::regex::makeMaybe(
            slim::regex::makeLit('a')
        ),
        slim::regex::makeOnePlus(
            slim::regex::makeLit('b')
        )
    );
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);

    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("group then literal", "[slim]")
{
    std::string pattern = "(ab)c";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeConcat(
        slim::regex::makeConcat(
            slim::regex::makeLit('a'),
            slim::regex::makeLit('b')
        ),
        slim::regex::makeLit('c')
    );
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);

    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("complex expression", "[slim]")
{
    std::string pattern = "(ab(c|x(de|fg)|h))+";
    std::unique_ptr<slim::regex::Node> expected = slim::regex::makeOnePlus(
        slim::regex::makeConcat(
            slim::regex::makeConcat(
                slim::regex::makeLit('a'),
                slim::regex::makeLit('b')
            ),
            slim::regex::makeUnion(
                slim::regex::makeUnion(
                    slim::regex::makeLit('c'),
                    slim::regex::makeConcat(
                        slim::regex::makeLit('x'),
                        slim::regex::makeUnion(
                            slim::regex::makeConcat(
                                slim::regex::makeLit('d'),
                                slim::regex::makeLit('e')
                            ),
                            slim::regex::makeConcat(
                                slim::regex::makeLit('f'),
                                slim::regex::makeLit('g')
                            )
                        )
                    )
                ),
                slim::regex::makeLit('h')
            )
        )
    );
    std::unique_ptr<slim::regex::Node> actual = slim::regex::Parse(pattern);

    REQUIRE(treesEq(*expected, *actual));
}

TEST_CASE("invalid patterns", "[slim]")
{
    std::string patterns[] = {
        "",
        "(",
        ")",
        "|",
        "?",
        "*",
        "+",
        "ab)",
        "ab|",
        "ab()",
        "ab|)",
        "?",
        "a|*",
        "a++",
        "ab\\",
        "|abc",
        "(abc)|",
        "a||b",
        "(abc))"
    };

    for (std::string pattern : patterns)
    {
        REQUIRE_THROWS(slim::regex::Parse(pattern));
    }
}
