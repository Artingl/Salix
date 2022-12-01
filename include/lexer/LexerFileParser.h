#pragma once

#include <cstddef>
#include <string>

#include "core.h"
#include "LexerToken.h"

namespace slx::lexer {

    class LexerFileParser {
    public:
        LexerFileParser(std::vector<lexer::LexerToken>& tokens, size_t poolId, size_t depth=0, std::string module="main");
        ~LexerFileParser();

        static LexerTokenType multipleSign(LexerTokenType symbol, LexerTokenType nextSymbol, LexerTokenType thirdSymbol);

        void parse();

    private:
        core::PoolValue _contents;
        size_t _depth;
        std::vector<lexer::LexerToken>& _tokens;
        std::string _module;

    protected:
        // type == 1 >> digit
        // type == 0 >> alpha
        std::string _parseAlphaDigit(size_t* location, uint8_t type);

        void _skipSpaces(size_t *location, size_t *lineNumber, size_t *lineOffset);
    };
}
