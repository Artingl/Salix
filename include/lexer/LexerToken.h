#pragma once

#include <string>

#define LEXER_TOKEN_MAGIC 0xca54bd12

namespace slx::lexer {
    enum LexerTokenType {
        NONE = 0xffff,
        ID,
        STRING,
        INTEGER,
        FLOAT,

        FUNCTION_TYPE, // ->
        DOUBLE_PLUS, // ++
        DOUBLE_MINUS, // --
        DOUBLE_MORE_OR_EQUAL, // <=
        DOUBLE_LESS_OR_EQUAL, // >=
        CONDITION_EQUALS, // ==
        TRUE, // true
        FALSE, // false
        AND, // &&
        OR, // ||
        LEFT_SHIFT, // <<
        RIGHT_SHIFT, // >>
        COMMENT, //

        PLUS_EQUALS, // +=
        MINUS_EQUALS, // -=
        MULTIPLY_EQUALS, // *=
        DIVIDE_EQUALS, // /=
        AND_EQUALS, // &=
        OR_EQUALS, // |=
        EUCLIDEAN_DIVISION_EQUALS, // %=
        RIGHT_SHIFT_EQUALS, // >>=
        LEFT_SHIFT_EQUALS, // <<=

        PLUS_SIGN = '+',
        MINUS_SIGN = '-',
        MULTIPLY_SIGN = '*',
        DIVIDE_SIGN = '/',
        EQUALS_SIGN = '=',
        MORE_SIGN = '<',
        LESS_SIGN = '>',
        OPEN_BRACKET = '(',
        CLOSE_BRACKET = ')',
        COMMA = ',',
        OPEN_BRACE = '{',
        CLOSE_BRACE = '}',
        OPEN_SQUARE_BRACKET = '[',
        CLOSE_SQUARE_BRACKET = ']',
        AMPERSAND_SIGN = '&',
        VERTICAL_BAR_SIGN = '|',
        NOT_SIGN = '!',
        PERCENT_SIGN = '%',
        SEMICOLON_SIGN = ';',
        COLON_SIGN = ':',
        DOUBLE_QUOTE = '"',
        DOT = '.',
        QUOTE = '\'',
    };

    struct TokenTypesList {
        LexerTokenType type0;
        LexerTokenType type1;
        LexerTokenType type2;
        LexerTokenType type3;
    };

    struct LexerToken {
        LexerTokenType type;
        std::string value;
        std::string module;
        std::string filePath;

        size_t poolId;
        size_t lineNumber;
        size_t lineOffset;
        size_t depth;

        uint32_t magic;
    };

    std::string getLocation(const LexerToken &token);
    std::string getLexerTypeName(LexerTokenType type);

}
