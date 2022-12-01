#pragma once

#include "lexer/LexerToken.h"
#include "core.h"

namespace slx::lexer {
    class Lexer {
    public:
        Lexer(const std::string& initialFile, const std::string& initialFileLocation);
        ~Lexer();

        std::vector<lexer::LexerToken> parse();

    private:
        size_t _initialFilePool;

    };

}
