#include <list.h>

List*createList()
{
    List*list = (List*)malloc(sizeof(List));
    list->po = 0;
    return list;
}

ListItem*createItem(struct AST*ast, ...)
{
    va_list args;
    va_start(args, ast);
    
    ListItem*item = (ListItem*)malloc(sizeof(ListItem));
    item->list = createList();
    item->ast = ast;
    
    item->flags = va_arg(args, uint32_t);
    item->data0 = va_arg(args, char*);
    item->data1 = va_arg(args, char*);
    item->data2 = va_arg(args, char*);
    item->data3 = va_arg(args, char*);
    item->data4 = va_arg(args, char*);
    item->data5 = va_arg(args, char*);

    va_end(args);

    return item;
}

ListItem*getListItem(List*list, uint64_t index)
{
    if (list->po < index)
        return NULL;
    return list->items[index];
}

uint64_t addListItem(List*list, ListItem*item)
{
    if (list->po == 0)
    {
        list->items = (ListItem**)calloc(++list->po, sizeof(ListItem));
        list->items[0] = item;
        return 0;
    }

    ListItem**copy = list->items;
    list->items = (ListItem**)calloc(++list->po, sizeof(ListItem));

    for (int i = 0; i < list->po - 1; i++)
        list->items[i] = copy[i];
    list->items[list->po - 1] = item;
    free(copy);

    return list->po - 1;
}

