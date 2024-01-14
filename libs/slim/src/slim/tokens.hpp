#pragma once

#include <string>
#include <vector>

namespace slim
{
    enum TokenType {
        KeywordProperty,
        KeywordShared,
        KeywordFeature,
        OpOr,
        OpAnd,
        OpEq,
        OpNeq,
        OpGt,
        OpLt,
        OpGe,
        OpLe,
        OpAdd,
        OpSub,
        OpMul,
        OpDiv,
        OpMod,
        OpBang,
        OpAssign,
        OpenParen,
        CloseParen,
        OpenBracket,
        CloseBracket,
        OpenBrace,
        CloseBrace,
        Dot,
        Comma,
        Semicolon,
        DataType,
        BoolLiteral,
        NumericLiteral,
        StringLiteral,
        Identifier,
        TagIdentifier
    };

    extern const std::vector<std::string> patterns;
}
