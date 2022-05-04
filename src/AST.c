#include <AST.h>
#include <token.h>


AST*createAST(uint16_t type)
{
    AST*ast = (AST*)malloc(sizeof(AST));
    ast->list = createList();
    ast->args = createList();
    ast->type = type;

    return ast;
}
