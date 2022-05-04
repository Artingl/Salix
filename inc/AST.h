#pragma once

#include <utils.h>
#include <list.h>

enum {
    AST_ARRAY_ARG_FLAG = 1 << 0,
    
    // next arguments are going to be like: second = 1 << 1, third = 1 << 2, etc.
};

enum {
    AST_STATEMENT           = 0,
    AST_JSON                = 1,
    AST_NUMBER              = 2,
    AST_STRING              = 3,

    AST_FN                  = 4,
    AST_VAR                 = 5,
    AST_CALL                = 6,

    AST_IF                  = 7,
    AST_ELSE                = 8,
    AST_FOR                 = 9,
    AST_WHILE               = 10,
    AST_RETURN              = 11,

    AST_ARRAY_INDEX         = 12,

    // math operations
    AST_OP_ADD              = 13,                // +
    AST_OP_SUB              = 14,                // -
    AST_OP_MULT             = 15,                // *
    AST_OP_DIV              = 16,                // /
    AST_OP_AMPER            = 17,                // &
    AST_OP_VB               = 18,                // |
    AST_OP_REM_OF_DIV       = 19,                // %
    AST_OP_NOT              = 20,                // !
    AST_OP_MORE             = 21,                // > 
    AST_OP_LESS             = 22,                // <
    AST_OP_EQU              = 23,                // =
    AST_OPEN_BRACKET        = 24,                // ( 
    AST_CLOSE_BRACKET       = 25,                // )

    AST_OP_AND              = 26,                // AND 
    AST_OP_OR               = 27,                // OR

    AST_OP_ITEM_EQUAL       = 28,                // ==
    AST_OP_ITEM_NOT_EQUAL   = 29,                // !=

    AST_EXPRESSION          = 30,
    AST_IF_EXPRESSION       = 31,

    AST_CHANGE_VAR          = 32,

    AST_POS_PRE             = 33,                // ~~VAR
    AST_POS_POST            = 34,                // VAR~~

    AST_ELIF                = 35,

};

struct If_struct {
    struct List*list;
    uint64_t if_id;
    bool alone;
};

typedef struct AST {
    struct List*list;
    struct List*args;
    
    struct AST*cond0;
    struct AST*cond1;
    struct AST*cond2;
    
    uint16_t type;
    uint16_t sub_type;
    uint16_t position;
    char*name;
    int64_t number;

    struct If_struct if_struct;

} AST;

AST*createAST(uint16_t type);
void printAST(struct List*list, int layer);

