#pragma once

#include <utils.h>
#include <AST.h>
#include <list.h>
#include <token.h>
#include <lexer.h>

typedef struct Parser {
    Lexer*lexer;
    Token*token;

} Parser;

Parser*createParser(Lexer*l);
AST*parserParseStatement(Parser*p, bool begining);
List*parserParseVariable(Parser*p, uint16_t stopToken0, uint16_t stopToken1, uint16_t stopToken2);
List*parserParseCall(Parser*p, bool parseLast);
List*parserParseJson(Parser*p);
List*parserParseFunArgs(Parser*p);
List*parserParseIf(Parser*p);

void parserParseOneToken(Parser*p, AST*body);
void parserParseID(Parser*p, AST*body);
void parserParseToken(Parser*p);
bool parserEat(Parser*p, uint16_t type);

