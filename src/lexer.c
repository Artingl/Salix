#include <lexer.h>

Lexer*createLexer(InputData*code)
{
    Lexer*l = (Lexer*)malloc(sizeof(Lexer));
    l->code = code;
    l->pos = 0;

    return l;
}

Token*parseLexerDigit(Lexer*l)
{
    char*id = (char*)malloc(sizeof(char));
    int sz = 0;
    int po = l->pos;

    while (_isdigit(l->code->buffer[l->pos]))
    {
        sz++;
        char*id_ = id;
        id = (char*)calloc(sz, sizeof(char));
        for (int i = 0; i < sz-1; i++) id[i] = id_[i];
        id[sz-1] = l->code->buffer[l->pos];
        free(id_);
        l->pos++;
    }
    l->pos--;

    return createToken(id, TOKEN_NUM, po, 0);
}

Token*parseLexerAlpha(Lexer*l)
{
    char*id = (char*)malloc(sizeof(char));
    int sz = 0;
    int po = l->pos;

    while (_isalpha(l->code->buffer[l->pos]) || 
            _isdigit(l->code->buffer[l->pos]))
    {
        sz++;
        char*id_ = id;
        id = (char*)calloc(sz, sizeof(char));
        for (int i = 0; i < sz-1; i++) id[i] = id_[i];
        id[sz-1] = l->code->buffer[l->pos];
        free(id_);
        l->pos++;
    }
    l->pos--;

    if (strcmp((const char*)id, (const char*)"true") == 0)
        return createToken("1", TOKEN_NUM, po, 0);
    if (strcmp((const char*)id, (const char*)"false") == 0)
        return createToken("0", TOKEN_NUM, po, 0);

    return createToken(id, 
        strcmp((const char*)id, (const char*)"and") == 0 ? TOKEN_AND :
        strcmp((const char*)id, (const char*)"or") == 0 ? TOKEN_OR : TOKEN_ID, po, 0);
}

Token*parseLexerString(Lexer*l, char open)
{
    char*id = (char*)malloc(sizeof(char));
    int sz = 0;
    int po = l->pos;
    l->pos++;

    while (open != l->code->buffer[l->pos])
    {
        sz++;
        char*id_ = id;
        id = (char*)calloc(sz, sizeof(char));
        for (int i = 0; i < sz-1; i++) id[i] = id_[i];
        id[sz-1] = l->code->buffer[l->pos];
        free(id_);
        l->pos++;
    }

    return createToken(id, TOKEN_STRING, po, 0);
}

Token*parseLexer(Lexer*l)
{
    char chr = l->code->buffer[l->pos];
    if (chr == ' ' || chr == '\n' || chr == '\t')
    {
        ++l->pos;
        return parseLexer(l);
    }
    
    
    if (_isdigit(chr))
        return parseLexerDigit(l);
    if (_isalpha(chr))
        return parseLexerAlpha(l);
    
    uint16_t type;
    switch (chr)
    {
    case '(':
        type = TOKEN_LEFT_BRACKET;
        break;
    case ')':
        type = TOKEN_RIGHT_BRACKET;
        break;
    case '[':
        type = TOKEN_LEFT_SQUARE_BRACKET;
        break;
    case ']':
        type = TOKEN_RIGHT_SQUARE_BRACKET;
        break;
    case '{':
        type = TOKEN_LEFT_BRACE;
        break;
    case '}':
        type = TOKEN_RIGHT_BRACE;
        break;
    case ';':
        type = TOKEN_SEMICOLON;
        break;
    case ':':
        type = TOKEN_COLON;
        break;
    case '"':
    case '\'':
        type = TOKEN_QUOTE;
        break;
    case ',':
        type = TOKEN_COMMA;
        break;
    case '!': {
        type = TOKEN_NOT;
        if (l->code->buffer[l->pos + 1] == '=') { type = TOKEN_ITEM_NOT_EQUAL; l->pos++; };
    } break;
    case '=': {
        type = TOKEN_EQUALS;
        if (l->code->buffer[l->pos + 1] == '=') { type = TOKEN_ITEM_EQUAL; l->pos++; };
    } break;
    case '+': {
        type = TOKEN_PLUS;
        if (l->code->buffer[l->pos + 1] == '=') { type = TOKEN_ITEM_PLUS; l->pos++; };
        if (l->code->buffer[l->pos + 1] == '+') { type = TOKEN_ITEM_PLUS_PLUS; l->pos++; };
    } break;
    case '-': {
        type = TOKEN_MINUS;
        if (l->code->buffer[l->pos + 1] == '=') { type = TOKEN_ITEM_MINUS; l->pos++; };
        if (l->code->buffer[l->pos + 1] == '-') { type = TOKEN_ITEM_MINUS_MINUS; l->pos++; };
    } break;
    case '.':
        type = TOKEN_DOT;
        break;
    case '|': {
        type = TOKEN_VB;
        if (l->code->buffer[l->pos + 1] == '=') { type = TOKEN_ITEM_OR; l->pos++; };
    } break;
    case '/': {
        type = TOKEN_SLASH;
        if (l->code->buffer[l->pos + 1] == '=') { type = TOKEN_ITEM_DIV; l->pos++; };
    } break;
    case '<':
        type = TOKEN_LEFT_ARRAW;
        break;
    case '>':
        type = TOKEN_RIGHT_ARRAW;
        break;
    case '^':
        type = TOKEN_UP_ARROW;
        break;
    case '*': {
        type = TOKEN_MULTIPLY;
        if (l->code->buffer[l->pos + 1] == '=') { type = TOKEN_ITEM_MULTIPLY; l->pos++; };
    } break;
    case '&': {
        type = TOKEN_AMPER;
        if (l->code->buffer[l->pos + 1] == '=') { type = TOKEN_ITEM_AND; l->pos++; };
    } break;
    case '%':
        type = TOKEN_PERCENT;
        break;
    case '#':
        type = TOKEN_HASHTAG;
        break;
    case '\0':
        type = TOKEN_EOF;
        break;
    
    default:
        if (1)
            {
                if (debug())
                    throwE(l->code->path, parseLexerLine(l), parseLexerPos(l), "Undefined character '%c' (ASCII: %d)!", chr, chr);
                exit(1);
            }
        break;
    }

    if (type == TOKEN_QUOTE)
        return parseLexerString(l, chr);

    return createToken(charToPtr(chr), type, l->pos, 0);
}

uint64_t parseLexerLine(Lexer*l)
{
    uint64_t lines = 1;
    for (int i = 0; i < l->code->size; i++)
    {
        if (l->pos == i)
            break;
        if (l->code->buffer[i] == '\n')
            lines++;
    }
    return lines;
}

uint64_t parseLexerPos(Lexer*l)
{
    uint64_t lines = 1;
    uint64_t pos = 0;
    for (int i = 0; i < l->code->size; i++)
    {
        if (l->pos == i)
            pos = l->pos - lines;
        if (l->code->buffer[i] == '\n')
            lines = i;
    }
    return pos;
}

