#pragma once

#include "types.hpp"

namespace slim::operators
{
    enum Operator
    {
        Assign,
        Or,
        And,
        Eq,
        Neq,
        Gt,
        Lt,
        Ge,
        Le,
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        Not,
        Index,
    };

    const std::string &toString(Operator op);
    Operator fromString(const std::string &s);
}
