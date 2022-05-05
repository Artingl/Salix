#include <parser.h>


Parser*createParser(Lexer*l)
{
    Parser*p = (Parser*)malloc(sizeof(Parser));
    p->lexer = l;
    parserParseToken(p);
    return p;
}

AST*parserParseStatement(Parser*p, bool begining)
{
    if (!begining || p->token->type == TOKEN_LEFT_BRACE)
    {
        parserEat(p, TOKEN_LEFT_BRACE);
        parserParseToken(p);
    }

    AST*body = createAST(AST_STATEMENT);
    while (p->lexer->pos <= p->lexer->code->size - 1)
    {
        parserParseOneToken(p, body);
        
        if(!begining) {
            if (p->token->type == TOKEN_RIGHT_BRACE)
                break;
        }
    }
    
    if (!begining)
    {
        parserEat(p, TOKEN_RIGHT_BRACE);
        parserParseToken(p);
    }
    
    return body;
}

void parserParseOneToken(Parser*p, AST*body) {
    if (p->token->type == TOKEN_ID)
    {
        parserParseID(p, body); 
    }
    else {
        AST*objAST = createAST(AST_EXPRESSION);
        objAST->list = parserParseVariable(p, TOKEN_SEMICOLON, TOKEN_SEMICOLON, TOKEN_SEMICOLON);
        addListItem(body->list, createItem(objAST));
    }
}

List*parserParseFunArgs(Parser*p)
{
    List*list = createList();
    bool arrayArgument = 0;
    char*arrayArgumentName;
    while (p->token->type != TOKEN_RIGHT_BRACKET)
    {
        if (arrayArgument)
        {
            if (debug())
                throwE(p->lexer->code->path, parseLexerLine(p->lexer), parseLexerPos(p->lexer), "Unexpected argument after array argument (*%s)", arrayArgumentName);
            exit(1);
        }
        else if (p->token->type == TOKEN_MULTIPLY)
        {   // array argument
            parserParseToken(p);
            arrayArgumentName = p->token->value;
            arrayArgument = 1;
        }

        addListItem(list, createItem(NULL, arrayArgument ? AST_ARRAY_ARG_FLAG : 0, p->token->value));
        parserParseToken(p);
        if (p->token->type == TOKEN_RIGHT_BRACKET)
            break;
        parserEat(p, TOKEN_COMMA);
        parserParseToken(p);
    }
    return list;
}

List*parserParseJson(Parser*p)
{   // array (not supported for now)
    while(p->token->type != TOKEN_RIGHT_BRACE)
    {
        parserParseToken(p);
    }

    parserParseToken(p);
    return createList();
}

List*parserParseCall(Parser*p, bool parseLast)
{
    List*list = createList();

    uint64_t openedBrackets = 1;
    while(openedBrackets > 0)
    {
        ListItem*item = createItem(NULL);
        item->list = parserParseVariable(p, TOKEN_COMMA, TOKEN_RIGHT_BRACKET, TOKEN_RIGHT_BRACKET);
        addListItem(list, item);
        if (p->token->type == TOKEN_LEFT_BRACKET)
        {
            ++openedBrackets;
            parserParseToken(p);
            continue;
        }
        else if (p->token->type == TOKEN_RIGHT_BRACKET)
        {
            --openedBrackets;
            parserParseToken(p);
            continue;
        }

        parserEat(p, TOKEN_COMMA);
        parserParseToken(p);
    }

    if (openedBrackets > 0)
        parserEat(p, TOKEN_RIGHT_BRACKET);
    if (parseLast)
        parserParseToken(p);

    return list;
}

List*parserParseIf(Parser*p)
{
    List*list = createList();

    while (p->token->type != TOKEN_RIGHT_BRACKET) {
        ListItem*expression = createItem(createAST(AST_IF_EXPRESSION));
        if (p->token->type == TOKEN_AND) {
            expression->ast->type = AST_OP_AND;
            parserParseToken(p);
        }
        else if (p->token->type == TOKEN_OR) {
            expression->ast->type = AST_OP_OR;
            parserParseToken(p);
        }
        else {
            expression->ast->list =  parserParseVariable(p, TOKEN_AND, TOKEN_OR, TOKEN_RIGHT_BRACKET);
        }

        addListItem(list, expression);
    }

    parserEat(p, TOKEN_RIGHT_BRACKET);
    parserParseToken(p);

    return list;
}

List*parserParseVariable(Parser*p, uint16_t stopToken0, uint16_t stopToken1, uint16_t stopToken2)
{
    List*list = createList();

    uint64_t openedBrackets = 0;
    while (p->token->type != stopToken0 && p->token->type != stopToken1 && p->token->type != stopToken2)
    {
        ListItem*item = createItem(createAST(AST_STATEMENT));
        if (p->token->type == TOKEN_LEFT_BRACKET)
        {
            ++openedBrackets;
            item->ast->type = AST_OPEN_BRACKET;
            addListItem(list, item);
            parserParseToken(p);
            continue;
        }
        else if (p->token->type == TOKEN_RIGHT_BRACKET)
        {
            --openedBrackets;
            item->ast->type = AST_CLOSE_BRACKET;
            addListItem(list, item);
            parserParseToken(p);
            continue;
        }
        else if (p->token->type == TOKEN_SEMICOLON)
        {
            break;
        }
        
        switch (p->token->type)
        {
            case TOKEN_ID:
                {
                    item->ast->name = p->token->value;

                    parserEat(p, TOKEN_ID);
                    parserParseToken(p);

                    if (p->token->type == TOKEN_LEFT_BRACKET)
                    {   // function calling
                        parserParseToken(p);
                        item->ast->type = AST_CALL;
                        item->ast->list = parserParseCall(p, false);
                    }
                    else { // getting variable value
                        item->ast->type = AST_VAR;
                    }
                    break;
                }

            case TOKEN_RIGHT_ARRAW:
                {
                    item->ast->type = AST_OP_MORE;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_LEFT_ARRAW:
                {
                    item->ast->type = AST_OP_LESS;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_EQUALS:
                {
                    item->ast->type = AST_OP_EQU;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_NUM:
                {
                    item->ast->type = AST_NUMBER;
                    item->ast->number = atoi(p->token->value);
                    parserParseToken(p);
                    break;
                }

            case TOKEN_STRING:
                {
                    item->ast->type = AST_STRING;
                    item->ast->name = p->token->value;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_LEFT_BRACE:
                {
                    item->ast->type = AST_JSON;
                    item->ast->list = parserParseJson(p);
                    break;
                }
            
            case TOKEN_PLUS:
                {
                    item->ast->type = AST_OP_ADD;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_MINUS:
                {
                    item->ast->type = AST_OP_SUB;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_MULTIPLY:
                {
                    item->ast->type = AST_OP_MULT;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_SLASH:
                {
                    item->ast->type = AST_OP_DIV;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_AMPER:
                {
                    item->ast->type = AST_OP_AMPER;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_VB:
                {
                    item->ast->type = AST_OP_VB;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_PERCENT:
                {
                    item->ast->type = AST_OP_REM_OF_DIV;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_NOT:
                {
                    item->ast->type = AST_OP_NOT;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_ITEM_EQUAL:
                {
                    item->ast->type = AST_OP_ITEM_EQUAL;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_ITEM_NOT_EQUAL:
                {
                    item->ast->type = AST_OP_ITEM_NOT_EQUAL;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_ITEM_MINUS_MINUS:
                {
                    item->ast->type = AST_OP_SUB;
                    addListItem(list, item);
                    item = createItem(createAST(AST_STATEMENT));
                    item->ast->type = AST_OP_SUB;
                    parserParseToken(p);
                    break;
                }

            case TOKEN_ITEM_PLUS_PLUS:
                {
                    item->ast->type = AST_OP_ADD;
                    addListItem(list, item);
                    item = createItem(createAST(AST_STATEMENT));
                    item->ast->type = AST_OP_ADD;
                    parserParseToken(p);
                    break;
                }
            
            case TOKEN_LEFT_SQUARE_BRACKET:
                {
                    parserParseToken(p);
                    item->ast->type = AST_ARRAY_INDEX;
                    item->ast->args = parserParseVariable(p, TOKEN_RIGHT_SQUARE_BRACKET, TOKEN_RIGHT_SQUARE_BRACKET, TOKEN_RIGHT_SQUARE_BRACKET);
                    parserEat(p, TOKEN_RIGHT_SQUARE_BRACKET);
                    parserParseToken(p);
                    break;
                }
                
            default:
                {
                    if (debug())
                        throwE(p->lexer->code->path, parseLexerLine(p->lexer), parseLexerPos(p->lexer), "Unexpected symbol '%s'!", tokenSymbol(p->token->type));
                    exit(1);
                }
        }

        addListItem(list, item);
    }

    if (openedBrackets > 0)
        parserEat(p, TOKEN_RIGHT_BRACKET);
    if (stopToken0 == TOKEN_SEMICOLON && stopToken1 == TOKEN_SEMICOLON)
        parserParseToken(p);
    
    return list;
}

void parserParseID(Parser*p, AST*body)
{
    AST*objAST;
    if (strcmp((const char*)p->token->value, (const char*)"fn") == 0)
    {   // function statement
        objAST = createAST(AST_FN);

        // parse function name
        parserParseToken(p);
        parserEat(p, TOKEN_ID);
        Token*funName = p->token;
        objAST->name = funName->value;

        // parse function args
        parserParseToken(p);
        parserEat(p, TOKEN_LEFT_BRACKET);
        parserParseToken(p);
        objAST->args = parserParseFunArgs(p);
        parserEat(p, TOKEN_RIGHT_BRACKET);
        parserParseToken(p);

        // parse function body
        // objAST->list = parserParseStatement(p, false)->list;
        
        if (p->token->type != TOKEN_LEFT_BRACE) {
            AST*h = createAST(AST_STATEMENT);
            parserParseOneToken(p, h);
            objAST->list = h->list;
        }
        else 
            objAST->list = parserParseStatement(p, false)->list;
    }
    else if (strcmp((const char*)p->token->value, (const char*)"if") == 0)
    {
        parserParseToken(p);
        parserEat(p, TOKEN_LEFT_BRACKET);
        parserParseToken(p);
        objAST = createAST(AST_IF);
        objAST->args = parserParseIf(p);
        objAST->if_struct.list = createList();
        objAST->if_struct.alone = true;

        if (p->token->type != TOKEN_LEFT_BRACE) {
            AST*h = createAST(AST_STATEMENT);
            parserParseOneToken(p, h);
            objAST->list = h->list;
        }
        else 
            objAST->list = parserParseStatement(p, false)->list;
    }
    else if (strcmp((const char*)p->token->value, (const char*)"elif") == 0)
    {
elif_st:
        parserParseToken(p);
        parserEat(p, TOKEN_LEFT_BRACKET);
        parserParseToken(p);
        objAST = createAST(AST_ELIF);
        objAST->args = parserParseIf(p);

        if (p->token->type != TOKEN_LEFT_BRACE) {
            AST*h = createAST(AST_STATEMENT);
            parserParseOneToken(p, h);
            objAST->list = h->list;
        }
        else 
            objAST->list = parserParseStatement(p, false)->list;
        
        
        for (int i = body->list->po - 1; i >= 0; i--) {
            ListItem*item = body->list->items[i];
            
            if (item->ast->type == AST_IF) {
                item->ast->if_struct.alone = false;
                addListItem(item->ast->if_struct.list, createItem(objAST));
                return;
            }
        }

        printE("Before an 'elif' statement should be 'if'!");
        exit(1);
    }
    else if (strcmp((const char*)p->token->value, (const char*)"else") == 0)
    {
        parserParseToken(p);
        if (strcmp((const char*)p->token->value, (const char*)"if") == 0) {
            goto elif_st;
        }

        objAST = createAST(AST_ELSE);

        if (p->token->type != TOKEN_LEFT_BRACE) {
            AST*h = createAST(AST_STATEMENT);
            parserParseOneToken(p, h);
            objAST->list = h->list;
        }
        else 
            objAST->list = parserParseStatement(p, false)->list;
        
        int elses = 0;
        for (int i = body->list->po - 1; i >= 0; i--) {
            ListItem*item = body->list->items[i];
            
            if (item->ast->type == AST_IF) {
                item->ast->if_struct.alone = false;
                addListItem(item->ast->if_struct.list, createItem(objAST));
                return;
            }
        }

        printE("Before an 'else' statement should be 'if'!");
        exit(1);
    }
    else if (strcmp((const char*)p->token->value, (const char*)"for") == 0)
    {
        parserParseToken(p);
        parserEat(p, TOKEN_LEFT_BRACKET);
        parserParseToken(p);
        objAST = createAST(AST_FOR);

        objAST->cond0 = createAST(AST_STATEMENT);
        objAST->cond1 = createAST(AST_EXPRESSION);
        objAST->cond2 = createAST(AST_STATEMENT);

        objAST->cond2->parent_type = AST_FOR;

        parserParseOneToken(p, objAST->cond0);
        parserParseOneToken(p, objAST->cond1);
        parserParseOneToken(p, objAST->cond2);

        parserEat(p, TOKEN_RIGHT_BRACKET);
        parserParseToken(p);

        if (p->token->type != TOKEN_LEFT_BRACE) {
            AST*h = createAST(AST_STATEMENT);
            parserParseOneToken(p, h);
            objAST->list = h->list;
        }
        else 
            objAST->list = parserParseStatement(p, false)->list;
    }
    else if (strcmp((const char*)p->token->value, (const char*)"while") == 0)
    {
        parserParseToken(p);
        parserEat(p, TOKEN_LEFT_BRACKET);
        parserParseToken(p);
        objAST = createAST(AST_WHILE);
        objAST->args = parserParseIf(p);

        if (p->token->type != TOKEN_LEFT_BRACE) {
            AST*h = createAST(AST_STATEMENT);
            parserParseOneToken(p, h);
            objAST->list = h->list;
        }
        else 
            objAST->list = parserParseStatement(p, false)->list;
    }
    else if (strcmp((const char*)p->token->value, (const char*)"let") == 0)
    {
        parserParseToken(p);
        parserEat(p, TOKEN_ID);
        Token*objName = p->token;
        parserParseToken(p);
        if (p->token->type == TOKEN_SEMICOLON) {
            objAST = createAST(AST_VAR);
            ListItem*item = createItem(createAST(AST_STATEMENT));
            item->ast->type = AST_NUMBER;
            item->ast->number = 0;
            addListItem(objAST->list, item);
            parserParseToken(p);
            objAST->name = objName->value;
        }
        else {
            parserEat(p, TOKEN_EQUALS);
            parserParseToken(p);
            objAST = createAST(AST_VAR);
            objAST->list = parserParseVariable(p, TOKEN_SEMICOLON, TOKEN_SEMICOLON, TOKEN_SEMICOLON);
            objAST->name = objName->value;
        }
    }
    else if (strcmp((const char*)p->token->value, (const char*)"return") == 0)
    {
        parserParseToken(p);
        objAST = createAST(AST_RETURN);
        objAST->list = parserParseVariable(p, TOKEN_SEMICOLON, TOKEN_SEMICOLON, TOKEN_SEMICOLON);
    }
    else {
        parserEat(p, TOKEN_ID);
        Token*objName = p->token;
        parserParseToken(p);
        
        if (p->token->type == TOKEN_EQUALS || p->token->type == TOKEN_ITEM_PLUS || p->token->type == TOKEN_ITEM_MINUS ||
            p->token->type == TOKEN_ITEM_OR || p->token->type == TOKEN_ITEM_DIV || p->token->type == TOKEN_ITEM_MULTIPLY ||
                p->token->type == TOKEN_ITEM_AND || p->token->type == TOKEN_ITEM_NOT_EQUAL || p->token->type == TOKEN_ITEM_PLUS_PLUS || 
                    p->token->type == TOKEN_ITEM_MINUS_MINUS)
        {   // variable
            int t = TOKEN_SEMICOLON;

            if (body->parent_type == AST_FOR) {
                t = TOKEN_RIGHT_BRACKET;
            }

            uint16_t sub_type = p->token->type;
            parserParseToken(p);
            objAST = createAST(AST_CHANGE_VAR);
            objAST->sub_type = sub_type;
            objAST->position = AST_POS_POST;
            objAST->list = parserParseVariable(p, t, t, t);
        }
        else if(p->token->type == TOKEN_LEFT_BRACKET) { // call
            parserParseToken(p);
            objAST = createAST(AST_CALL);
            objAST->list = parserParseCall(p, true);
        }
        else {
            if (debug())
                throwE(p->lexer->code->path, parseLexerLine(p->lexer), parseLexerPos(p->lexer), "Unexpected symbol '%s'! Expected either 'let' or '%s'", tokenSymbol(p->token->type), tokenSymbol(TOKEN_LEFT_BRACKET));
            exit(1);
        }
        
        objAST->name = objName->value;
    }

    addListItem(body->list, createItem(objAST));
}

void parserParseToken(Parser*p)
{
    p->token = parseLexer(p->lexer);
    ++p->lexer->pos;
}

bool parserEat(Parser*p, uint16_t type)
{
    if (p->token->type != type)
    {
        if (debug())
            throwE(p->lexer->code->path, parseLexerLine(p->lexer), parseLexerPos(p->lexer), "Unexpected symbol '%s'! Expected '%s'", tokenSymbol(p->token->type), tokenSymbol(type));
        exit(1);
        return false;
    }
    return true;
}


