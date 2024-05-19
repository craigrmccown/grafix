#include <map>
#include <memory>
#include <string>
#include <variant>
#include "operators.hpp"
#include "types.hpp"

namespace slim::operators
{
    // Operators are implemented as static constants as they cannot be defined
    // by the user
    const std::string
        assignStr = "=",
        orStr = "||",
        andStr = "&&",
        eqStr = "==",
        neqStr = "!=",
        gtStr = ">",
        ltStr = "<",
        geStr = ">=",
        leStr = "<=",
        addStr = "+",
        subStr = "-",
        mulStr = "*",
        divStr = "/",
        modStr = "%",
        notStr = "!",
        indexStr = "[]";

    const std::map<std::string, Operator> operatorIndex {
        { assignStr, Operator::Assign },
        { orStr, Operator::Or },
        { andStr, Operator::And },
        { eqStr, Operator::Eq },
        { neqStr, Operator::Neq },
        { gtStr, Operator::Gt },
        { ltStr, Operator::Lt },
        { geStr, Operator::Ge },
        { leStr, Operator::Le },
        { addStr, Operator::Add },
        { subStr, Operator::Sub },
        { mulStr, Operator::Mul },
        { divStr, Operator::Div },
        { modStr, Operator::Mod },
        { notStr, Operator::Not },
        { indexStr, Operator::Index },
    };

    const std::string &toString(Operator op)
    {
        switch (op)
        {
            case Operator::Assign:
                return assignStr;
            case Operator::Or:
                return orStr;
            case Operator::And:
                return andStr;
            case Operator::Eq:
                return eqStr;
            case Operator::Neq:
                return neqStr;
            case Operator::Gt:
                return gtStr;
            case Operator::Lt:
                return ltStr;
            case Operator::Ge:
                return geStr;
            case Operator::Le:
                return leStr;
            case Operator::Add:
                return addStr;
            case Operator::Sub:
                return subStr;
            case Operator::Mul:
                return mulStr;
            case Operator::Div:
                return divStr;
            case Operator::Mod:
                return modStr;
            case Operator::Not:
                return notStr;
            case Operator::Index:
                return indexStr;
        }
    }

    Operator fromString(const std::string &s)
    {
       std::map<std::string, Operator>::const_iterator it = operatorIndex.find(s);

        if (it == operatorIndex.end())
        {
            throw std::runtime_error("Unrecognized operator: " + s);
        }

        return (*it).second;
    }
}
