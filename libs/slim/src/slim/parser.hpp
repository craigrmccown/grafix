#include "lexer.hpp"

namespace slim
{
    enum TokenType {
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
        Dot,
        BoolLiteral,
        NumericLiteral,
        Identifier,
        DataType,
        Comma,
    };

    class Parser
    {
        public:
        Parser(TokenIter &tokens);

        void Parse();

        private:
        Token current;
        TokenIter &tokens;

        bool check(TokenType);
        void expect(TokenType);

        void pExpr();
        void pOrExpr();
        void pAndExpr();
        void pEqualityExpr();
        void pComparisonExpr();
        void pAddExpr();
        void pMulExpr();
        void pPrefixExpr();
        void pPostfixExpr();
        void pArgList();
        void pValueExpr();
    };
}
