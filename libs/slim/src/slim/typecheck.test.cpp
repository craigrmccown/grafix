#include <fstream>
#include <memory>
#include <catch2/catch_test_macros.hpp>
#include <testutils/dirgen.hpp>
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "tokens.hpp"
#include "typecheck.hpp"
#include "types.hpp"

TEST_CASE("passes typechecking for correct types", "[typecheck]")
{
    auto entry = GENERATE(testutils::dir("libs/slim/test/typecheck/pass"));
    REQUIRE(entry.is_regular_file());

    std::ifstream ifs(entry.path(), std::ios::binary);
    ifs >> std::noskipws;

    REQUIRE(ifs.is_open());

    slim::Lexer lex(
        slim::patterns,
        std::istream_iterator<char>(ifs),
        std::istream_iterator<char>()
    );

    slim::Parser parser(lex);
    std::unique_ptr<slim::ast::Program> ast = parser.Parse();

    slim::types::SymbolTable symbols;
    slim::types::TypeRegistry types;
    CHECK_NOTHROW(slim::typecheck::run(*ast, symbols, types));
}

TEST_CASE("fails typechecking for incorrect types", "[typecheck]")
{
    auto entry = GENERATE(testutils::dir("libs/slim/test/typecheck/fail"));
    REQUIRE(entry.is_regular_file());

    std::ifstream ifs(entry.path(), std::ios::binary);
    ifs >> std::noskipws;

    REQUIRE(ifs.is_open());

    slim::Lexer lex(
        slim::patterns,
        std::istream_iterator<char>(ifs),
        std::istream_iterator<char>()
    );

    slim::Parser parser(lex);
    std::unique_ptr<slim::ast::Program> ast = parser.Parse();

    slim::types::SymbolTable symbols;
    slim::types::TypeRegistry types;

    // TODO: Use CHECK_THROWS_AS and check error types or return errors as value
    // instead of throwing
    CHECK_THROWS(slim::typecheck::run(*ast, symbols, types));
}
