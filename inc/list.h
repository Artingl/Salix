#pragma once

#include <utils.h>
#include <AST.h>
#include <map.h>

typedef struct List;

typedef struct ListItem {
    struct AST*ast;
    struct List*list;

    char*data0;
    char*data1;
    char*data2;
    char*data3;
    char*data4;
    char*data5;

    uint32_t flags;

    Map *parent_local_variables;
    Map *parent_high_local_variables;

} ListItem;

typedef struct List {
    ListItem**items;
    uint64_t po;
    
} List;

List*createList();
ListItem*createItem(struct AST*ast, ...);

ListItem*getListItem(List*list, uint64_t index);
uint64_t addListItem(List*list, ListItem*item);

