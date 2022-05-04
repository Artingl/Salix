#include <AST.h>
#include <token.h>

// debug stuff
char*parseASTFunArgs(AST*ast);
char*parseASTVarValue(List*list);
char*parseASTCallValue(AST*ast);
char *repeatStr (char *str, size_t count);

void printAST(struct List*list, int layer)
{
    for (int i = 0; i < list->po; i++)
    {
        ListItem*item = list->items[i];
        uint16_t astType = item->ast->type;
        if (layer > 0)
            printf("%s", repeatStr("\t", layer));
        switch (astType)
        {
            case AST_FN:
                {
                    printf("fn %s (%s): %llu\n", item->ast->name, parseASTFunArgs(item->ast), item->ast->list->po);
                    printAST(item->ast->list, layer + 1);
                    break;
                }

            case AST_VAR:
                {
                    printf("%s = %s\n", item->ast->name, parseASTVarValue(item->ast->list));
                    break;
                }

            case AST_CALL:
                {
                    printf("%s (%s)\n", item->ast->name, parseASTCallValue(item->ast));
                    break;
                }
        }
    }
}

char *repeatStr (char *str, size_t count) {
    if (count == 0) return NULL;
    char *ret = malloc (strlen (str) * count + count);
    if (ret == NULL) return NULL;
    strcpy (ret, str);
    while (--count > 0) {
        strcat (ret, " ");
        strcat (ret, str);
    }
    return ret;
}

char*parseASTCallValue(AST*ast)
{
    char*data = (char*)calloc(8192, 8);
    int cnt = 0;
    for (int i = 0; i < ast->list->po; i++)
    {
        char*d = parseASTVarValue(ast->list->items[i]->list);
        for (int j = 0; j < strlen(d); j++)
        {
            data[cnt++] = d[j];
        }
        data[cnt++] = ',';
    }
    data[cnt++] = '\0';
    return data;
}

char*parseASTVarValue(List*list)
{
    char*data = (char*)calloc(8192, 8);
    int cnt = 0;
    for (int i = 0; i < list->po; i++)
    {
        ListItem*item = list->items[i];
        switch (item->ast->type)
        {
            case AST_VAR:
            case AST_STRING:
                {
                    if (item->ast->type == AST_STRING)
                        data[cnt++] = '"';
                    char*n = item->ast->name;
                    for (int j = 0; j < strlen(n); j++)
                    {
                        data[cnt++] = n[j];
                    }
                    if (item->ast->type == AST_STRING)
                        data[cnt++] = '"';
                    break;
                }
            
            case AST_CALL:
                {
                    char*n = parseASTCallValue(item->ast);
                    for (int j = 0; j < strlen(item->ast->name); j++)
                        data[cnt++] = item->ast->name[j];
                    data[cnt++] = '(';
                    for (int j = 0; j < strlen(n); j++)
                        data[cnt++] = n[j];
                    data[cnt++] = ')';
                    break;
                }

            case AST_NUMBER:
                {
                    char*n = parseStrInt(item->ast->number);
                    for (int j = 0; j < strlen(n); j++)
                    {
                        data[cnt++] = n[j];
                    }
                    break;
                }
            
            case AST_OP_ADD:
                {
                    data[cnt++] = '+';
                    break;
                }

            case AST_OP_SUB:
                {
                    data[cnt++] = '-';
                    break;
                }

            case AST_OP_MULT:
                {
                    data[cnt++] = '*';
                    break;
                }

            case AST_OP_DIV:
                {
                    data[cnt++] = '/';
                    break;
                }

            case AST_OP_AMPER:
                {
                    data[cnt++] = '&';
                    break;
                }

            case AST_OP_VB:
                {
                    data[cnt++] = '|';
                    break;
                }

            case AST_OP_REM_OF_DIV:
                {
                    data[cnt++] = '%';
                    break;
                }

            case AST_OP_NOT:
                {
                    data[cnt++] = '!';
                    break;
                }
        }
    }
    data[cnt] = '\0';
    return data;
}

char*parseASTFunArgs(AST*ast)
{
    char*data = (char*)calloc(8192, 8);
    int cnt = 0;
    for (int i = 0; i < ast->args->po; i++)
    {
        char*str = ast->args->items[i]->data0;
        if (ast->args->items[i]->flags & AST_ARRAY_ARG_FLAG)
        {
            data[cnt++] = '*';
        }
        for (int j = 0; j < strlen(str); j++)
        {
            data[cnt++] = str[j];
        }
        data[cnt++] = ',';
    }
    data[cnt] = '\0';
    return data;
}

