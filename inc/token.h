#pragma once

#include <utils.h>

enum {
    TOKEN_ID,                       // id
    TOKEN_NUM,                      // Number
    TOKEN_STRING,                   // String
    TOKEN_LEFT_BRACKET,             // (
    TOKEN_RIGHT_BRACKET,            // )
    TOKEN_LEFT_SQUARE_BRACKET,      // [
    TOKEN_RIGHT_SQUARE_BRACKET,     // ]
    TOKEN_LEFT_BRACE,               // {
    TOKEN_RIGHT_BRACE,              // }
    TOKEN_SEMICOLON,                // ;
    TOKEN_COLON,                    // :
    TOKEN_QUOTE,                    // either ' or "
    TOKEN_COMMA,                    // ,
    TOKEN_NOT,                      // !
    TOKEN_VB,                       // |
    TOKEN_EQUALS,                   // =
    TOKEN_ITEM_PLUS_PLUS,           // ++
    TOKEN_ITEM_MINUS_MINUS,         // --
    TOKEN_ITEM_EQUAL,               // ==
    TOKEN_ITEM_NOT_EQUAL,           // !=
    TOKEN_ITEM_PLUS,                // +=
    TOKEN_ITEM_MINUS,               // -=
    TOKEN_ITEM_OR,                  // |=           
    TOKEN_ITEM_DIV,                 // /=
    TOKEN_ITEM_MULTIPLY,            // *=
    TOKEN_ITEM_AND,                 // &=
    TOKEN_PLUS,                     // +
    TOKEN_MINUS,                    // -
    TOKEN_DOT,                      // .
    TOKEN_SLASH,                    // /
    TOKEN_LEFT_ARRAW,               // <
    TOKEN_RIGHT_ARRAW,              // >
    TOKEN_UP_ARROW,                 // ^
    TOKEN_MULTIPLY,                 // *
    TOKEN_AMPER,                    // &
    TOKEN_PERCENT,                  // %
    TOKEN_HASHTAG,                  // #
    TOKEN_AND,                      // AND
    TOKEN_OR,                       // OR
    TOKEN_EOF,                      // \0
    TOKEN_NULL,                     // Unknown characters 
};


typedef struct TokenPosition {
    int line, pos;
} TokenPosition;

typedef struct Token {
    uint16_t type;
    char*value;

    struct TokenPosition*position;

} Token;

Token*createToken(char*value, uint16_t type, int l, int p);
char*tokenSymbol(uint16_t type);

