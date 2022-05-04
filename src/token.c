#include <token.h>

Token*createToken(char*value, uint16_t type, int l, int p)
{
    Token*t = (Token*)malloc(sizeof(Token));
    t->position = (TokenPosition*)malloc(sizeof(TokenPosition));

    t->type = type;
    t->position->line = l;
    t->position->pos = p;
    t->value = value;

    return t;
}

char*tokenSymbol(uint16_t type)
{
	if (type == TOKEN_ID)	return "ID";
	if (type == TOKEN_NUM)	return "Number";
	if (type == TOKEN_STRING)	return "String";
	if (type == TOKEN_LEFT_BRACKET)	return "(";
	if (type == TOKEN_RIGHT_BRACKET)	return ")";
	if (type == TOKEN_LEFT_SQUARE_BRACKET)	return "[";
	if (type == TOKEN_RIGHT_SQUARE_BRACKET)	return "]";
	if (type == TOKEN_LEFT_BRACE)	return "{";
	if (type == TOKEN_RIGHT_BRACE)	return "}";
	if (type == TOKEN_SEMICOLON)	return ";";
	if (type == TOKEN_COLON)	return ":";
	if (type == TOKEN_QUOTE)	return "\"";
	if (type == TOKEN_COMMA)	return ",";
	if (type == TOKEN_NOT)	return "!";
	if (type == TOKEN_EQUALS)	return "=";
	if (type == TOKEN_PLUS)	return "+";
	if (type == TOKEN_MINUS)	return "-";
	if (type == TOKEN_DOT)	return ".";
	if (type == TOKEN_SLASH)	return "/";
	if (type == TOKEN_VB)	return "|";
	if (type == TOKEN_LEFT_ARRAW)	return "<";
	if (type == TOKEN_RIGHT_ARRAW)	return ">";
	if (type == TOKEN_UP_ARROW)	return "^";
	if (type == TOKEN_MULTIPLY)	return "*";
	if (type == TOKEN_AMPER)	return "&";
	if (type == TOKEN_PERCENT)	return "%";
	if (type == TOKEN_HASHTAG)	return "#";
	if (type == TOKEN_AND)	return "AND";
	if (type == TOKEN_OR)	return "OR";
	if (type == TOKEN_ITEM_EQUAL)	return "==";
	if (type == TOKEN_ITEM_NOT_EQUAL)	return "!=";
	if (type == TOKEN_ITEM_PLUS_PLUS) return "++";
	if (type == TOKEN_ITEM_MINUS_MINUS) return "--";
	if (type == TOKEN_ITEM_PLUS) return "+=";
	if (type == TOKEN_ITEM_MINUS) return "-=";
	if (type == TOKEN_ITEM_OR) return "|=";
	if (type == TOKEN_ITEM_DIV) return "/=";
	if (type == TOKEN_ITEM_MULTIPLY) return "*=";
	if (type == TOKEN_ITEM_AND) return "&=";
	if (type == TOKEN_EOF)	return "\0";

	return "(NULL)";
}

