#include "lexer/LexerToken.h"


namespace slx::lexer {

    std::string getLocation(const LexerToken &token) {
        return "line " + std::to_string(token.lineNumber) + ", position " + std::to_string(token.lineOffset) + " in module " + token.module;
    }

    std::string getLexerTypeName(LexerTokenType type) {
        std::string value = "NULL";

        switch (type) {
            case ID:
                value = "identifier";
            case STRING:
                value = "string";
            case INTEGER:
                value = "integer";
            case FLOAT:
                value = "float";
            case FUNCTION_TYPE:
                value = "function type identifier (->)";
            case DOUBLE_PLUS:
                value = "++";
            case DOUBLE_MINUS:
                value = "--";
            case DOUBLE_MORE_OR_EQUAL:
                value = "<=";
            case DOUBLE_LESS_OR_EQUAL:
                value = ">=";
            case CONDITION_EQUALS:
                value = "==";
            case TRUE:
                value = "true";
            case FALSE:
                value = "false";
            case AND:
                value = "||";
            case OR:
                value = "&&";
            case LEFT_SHIFT:
                value = "<<";
            case RIGHT_SHIFT:
                value = ">>";
            case NONE:
                value = "NONE";
                break;
            case PLUS_EQUALS:
                value = "+=";
                break;
            case MINUS_EQUALS:
                value = "-=";
                break;
            case MULTIPLY_EQUALS:
                value = "*=";
                break;
            case DIVIDE_EQUALS:
                value = "/=";
                break;
            case AND_EQUALS:
                value = "&=";
                break;
            case OR_EQUALS:
                value = "|=";
                break;
            case EUCLIDEAN_DIVISION_EQUALS:
                value = "%=";
                break;
            case RIGHT_SHIFT_EQUALS:
                value = ">>=";
                break;
            case LEFT_SHIFT_EQUALS:
                value = "<<=";
                break;

            case COMMENT:
            case PLUS_SIGN:
            case MINUS_SIGN:
            case MULTIPLY_SIGN:
            case DIVIDE_SIGN:
            case EQUALS_SIGN:
            case MORE_SIGN:
            case LESS_SIGN:
            case OPEN_BRACKET:
            case CLOSE_BRACKET:
            case COMMA:
            case OPEN_BRACE:
            case CLOSE_BRACE:
            case OPEN_SQUARE_BRACKET:
            case CLOSE_SQUARE_BRACKET:
            case AMPERSAND_SIGN:
            case VERTICAL_BAR_SIGN:
            case NOT_SIGN:
            case PERCENT_SIGN:
            case SEMICOLON_SIGN:
            case COLON_SIGN:
            case DOUBLE_QUOTE:
            case DOT:
            case QUOTE:
                value = std::string(1, (char)type);
                break;
        }

        return value;
    }

}
