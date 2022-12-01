#include "lexer/Lexer.h"
#include "lexer/LexerFileParser.h"

namespace slx::lexer {

    Lexer::Lexer(const std::string &initialFile, const std::string &initialFileLocation) {
        _initialFilePool = core::addFileToPool(initialFile, initialFileLocation);
    }

    Lexer::~Lexer() {
    }

    std::vector<lexer::LexerToken> Lexer::parse() {
        std::vector<lexer::LexerToken> tokens;
        LexerFileParser(tokens, _initialFilePool).parse();
        return tokens;
    }
}
