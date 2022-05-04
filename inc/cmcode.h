#pragma once

#include <utils.h>
#include <AST.h>
#include <token.h>
#include <parser.h>
#include <list.h>
#include <lexer.h>

#define EXPRESSION_CPY(bf, nw) if (bf != NULL){ free((void*)bf); }      \
                                bf = malloc(strlen(nw));               \
                                strcpy(bf, nw);

#define ADD_TO_SECTION(secname, argument, ...) strcmp((const char*)secname, (const char*)"func") == 0 ? addFuncSection(argument, __VA_ARGS__) : addCodeSection(argument, __VA_ARGS__);

enum {
    IF_FLAG = 1,
};


char *initCmcode(ListItem*parent_item, uint8_t state, bool is_main, bool has_arguments);
char *parseExpression(ListItem*parent_item, ListItem*self_item, uint8_t state, uint8_t flags);
void parseCall(ListItem*parent_item, ListItem*item, uint8_t state);
void parseIfExpression(ListItem*parent_item, ListItem*item, uint8_t stat, uint64_t if_ide);
void load_expression_item(ListItem*parent_item,
                          ListItem*self_item, 
                          ListItem*operand_item, 
                          char*register_to_write, 
                          char*CURRS, 
                          uint8_t state, 
                          uint8_t flags);

void addCodeSection(const char*fmt, ...);
void addDataSection(const char*fmt, ...);
void addFuncSection(const char*fmt, ...);
void addExtrSection(const char*fmt, ...);
void addHdrSection(const char*fmt, ...);