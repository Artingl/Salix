#pragma once

#include <utils.h>
#include <token.h>

typedef struct Lexer {
    InputData*code;
    uint64_t pos;

} Lexer;

Lexer*createLexer(InputData*code);
Token*parseLexerAlpha(Lexer*l);
Token*parseLexerDigit(Lexer*l);
Token*parseLexerString(Lexer*l, char open);
Token*parseLexer(Lexer*lexer);

uint64_t parseLexerLine(Lexer*l);
uint64_t parseLexerPos(Lexer*l);

