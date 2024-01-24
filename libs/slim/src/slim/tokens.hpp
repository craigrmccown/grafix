#pragma once

#include <string>
#include <vector>

namespace slim
{
    enum TokenType {
        KeywordProperty,
        KeywordShared,
        KeywordFeature,
        KeywordShader,
        KeywordRequire,
        KeywordReturn,
        OpAssign,
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
        OpenParen,
        CloseParen,
        OpenBracket,
        CloseBracket,
        OpenBrace,
        CloseBrace,
        Dot,
        Comma,
        Semicolon,
        ShaderType,
        DataType,
        BoolLiteral,
        NumericLiteral,
        StringLiteral,
        Identifier,
        TagIdentifier
    };

    extern const std::vector<std::string> patterns;
}
