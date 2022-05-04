#include <cmcode.h>
#include <map.h>

char ast_expressions_table[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '+',
    '-',
    '*',
    '/',
    '&',
    '|',
    '%',
    '!',
    '>',
    '<',
    '=',
    '(',
    ')',
};

Map data_section_hashes;
Map *global_variables;

uint64_t total_if;
uint64_t counter;

char*header = ";; fasm | 32 bit\n"
              "format ELF\n";
char*data = "\nsection '.data' writeable\n\n";
char*extrns = "\n";
char*code = ";; macros\n"   
            "macro ncmp op1, op2\n"
            "{   ;; ZF = 1 if op1 != op2 \n"
            "    local eq\n"
            "    local neq\n"
            "    local end\n"
            "    push op1\n"
            "    cmp op1, op2\n"
            "	mov eax, 1\n"
            "    jnz neq\n"
            "eq:\n"
            "	cmp eax, 0\n"
            "    jmp end\n"
            "neq:\n"
            "	cmp eax, 1\n"
            "end:\n"
            "    pop op1\n"
            "}\n\n"
            "macro sdiv op1, op2\n"
            "{\n"
            "    push ecx\n"
            "    push edx\n"
            "    mov eax, op1\n"
            "    mov ecx, op2\n"
            "    mov edx, 0\n"
            "    idiv ecx\n"
            "    pop edx\n"
            "    pop ecx\n"
            "}\n\n"
            "macro lnot op1, op2\n"
            "{\n"
            "    local eq\n"
            "    local neq\n"
            "    local end\n"
            "    cmp op1, 0\n"
            "    jnz neq\n"
            "eq:\n"
            "    mov op1, 1\n"
            "    jmp end\n"
            "neq:\n"
            "    mov op1, 0\n"
            "end:\n"
            "}\n\n"
            "macro nop op1, op2\n"
            "{\n"
            "}\n\n"
            "section '.text' executable\n\n"
            "public _start\n"
            "_start:\n"
            "\tpush ebp\n"
            "\tmov ebp, esp\n"
            "\tmov edi, ebp\n"
            "\tpush 0\n";
char*funcs = "\n";

char*initCmcode(ListItem*parent_item, uint8_t state, bool is_main, bool has_arguments)
{   
    char CURRS[5] = "func\0";     // current section to write
    if (!state) 
        ARR_WRITE_STR(CURRS, ((char*)"code\0"), 5);

    Map func_variables;
    func_variables.fields = NULL;
    func_variables.size = 0;

    parent_item->parent_local_variables = &func_variables;

    if (is_main) {
        global_variables = &func_variables;
        parent_item->parent_high_local_variables = global_variables;
    }

    if (has_arguments)
        {
            for (int i = 0; i < parent_item->ast->args->po; i++)
            {
                int v = i * -4;
                map_push(&func_variables, createStrIndexField(parent_item->ast->args->items[i]->data0, (void*)(v - 8)));
            }
        }
    

    for (int i = 0; i < parent_item->ast->list->po; i++)
    {
        ListItem*item = parent_item->ast->list->items[i];
    
        switch (item->ast->type)
        {
            case AST_FN: {
                map_push(&func_variables, createStrIndexField(item->ast->name, (void*)(func_variables.size * 4 + 4)));
                item->parent_high_local_variables = &func_variables;

                addFuncSection("%s:\n", item->ast->name);
                addFuncSection("\tpush ebp\n");
                addFuncSection("\tmov ebp, esp\n\n");
                initCmcode(item, true, false, true);
                addFuncSection("\tmov esp, ebp\n");
                addFuncSection("\tpop ebp\n");
                addFuncSection("\tret\n");
            } break;

            case AST_RETURN: {
                // return means put the expression into the ECX register and then parse
                char *expression = parseExpression(parent_item, item, state, 0);
                addFuncSection("\tmov ecx, %s\n", expression);
                break;
            }

            case AST_CALL: {
                parseCall(parent_item, item, state);
            } break;

            case AST_EXPRESSION: {
                parseExpression(parent_item, item, state, 0);
            } break;

            case AST_FOR: {
                // TODO
            } break;

            case AST_WHILE: {
                //
            } break;

            case AST_IF: {
                int if_id = ++total_if;

                parseIfExpression(parent_item, item, state, if_id);

                if (item->ast->if_struct.alone) {
                    ADD_TO_SECTION(CURRS,
                        "\tjnz eoi_%d\n", if_id);
                }
                else {
                    ADD_TO_SECTION(CURRS,
                        "\tjnz if_%d_1\n", if_id);
                }
                
                ADD_TO_SECTION(CURRS,
                    "if_%d_0:\n", if_id);
                initCmcode(item, state, false, false);
                ADD_TO_SECTION(CURRS,
                    "\tjmp eoi_%d\n", if_id);

                for (int i = 0; i < item->ast->if_struct.list->po; i++) {
                    ListItem*if_item = item->ast->if_struct.list->items[i];

                    ADD_TO_SECTION(CURRS,
                        "if_%d_%d:\n", if_id, i + 1);

                    if (if_item->ast->type == AST_ELIF) {
                        ADD_TO_SECTION(CURRS, "\n", NULL);
                        parseIfExpression(parent_item, if_item, state, if_id);
                        ADD_TO_SECTION(CURRS,
                            "\tjnz if_%d_%d\n\n", if_id, i + 2);
                    }
                    
                    initCmcode(if_item, state, false, false);
                    ADD_TO_SECTION(CURRS,
                        "\tjmp eoi_%d\n", if_id);
                }
                
                ADD_TO_SECTION(CURRS,
                    "eoi_%d:\n", if_id);

            } break;

            case AST_CHANGE_VAR:
            case AST_VAR: {
                if (!state) {
                    addCodeSection("\n\t;; VAR '%s'\n", item->ast->name);
                }
                else {
                    addFuncSection("\n\t;; VAR '%s'\n", item->ast->name);
                }

                MapField_t *field_to_check;
                char *expression = parseExpression(parent_item, item, state, 0);
                uint64_t address = 0;
                uint16_t sub_type = TOKEN_EQUALS;
                
                if (item->ast->type != AST_CHANGE_VAR)
                    {
                        uint64_t expression_hash = atoi(str_hash(expression));
                        field_to_check = createStrIndexField(item->ast->name, (void*)(func_variables.size * 4 + 4));
                        address = func_variables.size * 4 + 4;
                    }
                    else {
                        MapField_t *result;
                        field_to_check = createStrIndexField(item->ast->name, (void*)"_");

                        bool global = false;

                        result = map_get(parent_item->parent_local_variables, field_to_check);
                        if (result == NULL)
                            {
                                result = map_get(global_variables, field_to_check);
                                global = true;
                            }

                        if (result != NULL) {                        
                            address = result->value;
                        }
                        else {
                            printE("Unable to find variable '%s' in this scope!", item->ast->name);
                            exit(1);
                        }

                        free((void*)field_to_check);
                        sub_type = item->ast->sub_type;
                    }

                char*instr = "mov";

                switch (sub_type) {
                    case TOKEN_ITEM_NOT_EQUAL:          { instr = "lnot"; break; }
                    case TOKEN_ITEM_PLUS:               { instr = "add"; break; }
                    case TOKEN_ITEM_MINUS:              { instr = "sub"; break; }
                    case TOKEN_ITEM_OR:                 { instr = "or"; break; }
                    case TOKEN_ITEM_DIV:                { instr = "sdiv"; break; }
                    case TOKEN_ITEM_MULTIPLY:           { instr = "imul"; break; }
                    case TOKEN_ITEM_AND:                { instr = "and"; break; }
                    case TOKEN_ITEM_PLUS_PLUS:          { instr = "inc"; break; }
                    case TOKEN_ITEM_MINUS_MINUS:        { instr = "dec"; break; }
                }

                if (!state) {
                    // we are in the code section
                    addCodeSection("\tsub esp, 4\n");
                    if (sub_type == TOKEN_ITEM_PLUS_PLUS || sub_type == TOKEN_ITEM_MINUS_MINUS) {
                        addCodeSection("\t%s dword [ebp - %u]\n", instr, address);
                    }
                    else {
                        addCodeSection("\t%s dword [ebp - %u], %s\n", instr, address, expression);
                    }
                    addCodeSection("\t;;\n\n");

                }
                else {
                    // it's functions section
                    addFuncSection("\tsub esp, 4\n");
                    if (sub_type == TOKEN_ITEM_PLUS_PLUS || sub_type == TOKEN_ITEM_MINUS_MINUS) {
                        addFuncSection("\t%s dword [ebp - %u]\n", instr, address);
                    }
                    else {
                        addFuncSection("\t%s dword [ebp - %u], %s\n", instr, address, expression);
                    }
                    addFuncSection("\t;;\n\n");
                }

                if (item->ast->type != AST_CHANGE_VAR)
                    map_push(&func_variables, field_to_check);
            } break;

        }
    }

    if (is_main)
    {
        // addExtrSection("extrn print\n");
        // addExtrSection("extrn exit\n");

        addCodeSection(
            "\n"
            "\t;; tell the kernel that we wanna exit\n"
            "\tmov eax, 1\n"
            "\tmov ebx, 0      ;; exit code\n"
            "\tint 0x80\n"
        );

        addFuncSection("\n\n");
        addDataSection("\n\n");

        uint64_t s = strlen(code) + strlen(funcs) + strlen(data) + strlen(extrns) + strlen(header) + 4;
        char*assembly_buffer = (malloc(s));
        for (int i = 0; i < s; i++)
            assembly_buffer[i] = 0;

        uint64_t offset = 0;

        for (int i = 0; i < strlen(header); i++)    assembly_buffer[offset++] = header[i];
        for (int i = 0; i < strlen(extrns); i++)    assembly_buffer[offset++] = extrns[i];
        for (int i = 0; i < strlen(code); i++)      assembly_buffer[offset++] = code[i];
        for (int i = 0; i < strlen(funcs); i++)     assembly_buffer[offset++] = funcs[i];
        for (int i = 0; i < strlen(data); i++)      assembly_buffer[offset++] = data[i];

        assembly_buffer[offset] = 0;

        return assembly_buffer;
    }

    return "";
}

void parseCall(ListItem*parent_item, ListItem*item, uint8_t state) {
    if (strcmp((const char*)item->ast->name, (const char*)"__asm__") == 0) {
        // asm macro
        for (int i = 0; i < item->ast->list->po; i++)
        {
            ListItem*argument_item = item->ast->list->items[i];
                                
            for (int i = 0; i < argument_item->list->po; i++)
            {
                ListItem*asm_item = argument_item->list->items[i];

                if (asm_item->ast->type != AST_STRING) {
                    printE("__asm__ statement can contain only strings!");
                    exit(1);
                }

                if (!state)     addCodeSection("\t%s\n", format_string(asm_item->ast->name));
                else            addFuncSection("\t%s\n", format_string(asm_item->ast->name));
            }
        }

        return;
    }
    
    MapField_t *field_to_check = createStrIndexField(item->ast->name, (void*)"_");
    MapField_t *result;

    result = map_get(parent_item->parent_high_local_variables, field_to_check);
    if (result == NULL) {
        printE("Unable to find function '%s' in this scope!", item->ast->name);
        exit(1);
    }
    
    free((void*)field_to_check);


    if (!state)
    {
        addCodeSection("\tpush eax\n");
        addCodeSection("\tpush ebx\n");
        addCodeSection("\tsub esp, %d\n", item->ast->list->po * 4);
    }
    else
    {
        addFuncSection("\tpush eax\n");
        addFuncSection("\tpush ebx\n");
        addFuncSection("\tsub esp, %d\n", item->ast->list->po * 4);
    }

                
    for (int i = 0; i < item->ast->list->po; i++)
    {
        ListItem*argument_item = item->ast->list->items[i];

        argument_item->ast = createAST(AST_STATEMENT);
        argument_item->ast->list = argument_item->list;
                    
        char *expression = parseExpression(parent_item, argument_item, state, 0);

        if (strlen(expression) != 1)
        {
            if (!state)     addCodeSection("\tmov dword [esp + %d], %s\n", i * 4, expression);
            else            addFuncSection("\tmov dword [esp + %d], %s\n", i * 4, expression);
        }
    }

    if (!state)     addCodeSection("\tcall %s\n"
                                   "\tadd esp, %d\n"
                                   "\tpop ebx\n"
                                   "\tpop eax\n",
                                   item->ast->name,
                                   item->ast->list->po * 4
                                  );
    else            addFuncSection("\tcall %s\n"
                                   "\tadd esp, %d\n"
                                   "\tpop ebx\n"
                                   "\tpop eax\n",
                                   item->ast->name,
                                   item->ast->list->po * 4
                                  );
}

void parseIfExpression(ListItem*parent_item, ListItem*item, uint8_t state, uint64_t if_id)
{
    char CURRS[5] = "func\0";     // current section to write
    if (!state) 
        ARR_WRITE_STR(CURRS, ((char*)"code\0"), 5);
    
    for (int i = 0; i < item->ast->args->po; i++)
    {
        ListItem*if_statement_item = item->ast->args->items[i];
        uint64_t local_counter = ++counter;

        if (if_statement_item->ast->type == AST_IF_EXPRESSION)
        {
            char*exp = parseExpression(parent_item, if_statement_item, state, IF_FLAG);

            ADD_TO_SECTION(CURRS,
                "\n\tjnz nl%d\n"
                "\tmov edx, 1\n"
                "nl%d:\n", local_counter, local_counter);
        }
        else if (if_statement_item->ast->type == AST_OP_AND) {
            uint64_t inside_if_id = total_if++;

            char*exp = parseExpression(parent_item, if_statement_item, state, inside_if_id);

            ADD_TO_SECTION(CURRS,
                "\n\tjnz nl%d\n"
                "\tmov edx, 1\n"
                "\tjnz nnl%d\n"
                "nl%d:\n"
                "\tmov edx, 0\n"
                "\tmov eax, 1\n"
                "nnl%d:\n", local_counter, local_counter, local_counter, local_counter);
        }
        else if (if_statement_item->ast->type == AST_OP_OR) {
            uint64_t inside_if_id = total_if++;

            parseIfExpression(parent_item, if_statement_item, state, inside_if_id);

            ADD_TO_SECTION(CURRS,
                "\n\tjnz nl%d\n"
                "\tmov edx, 1\n"
                "\tmov eax, 0\n"
                "nl%d:\n", local_counter, local_counter);
        }
    }
    
    uint64_t local_counter = ++counter;

    ADD_TO_SECTION(CURRS, 
        "\tcmp edx, 1\n" 
        "\tjnz nl%d\n"
        "\tcmp edx, 1\n"
        "\tmov edx, 0\n"
        "nl%d:\n", local_counter, local_counter
    );
}


char *parseExpression(ListItem*parent_item, ListItem*self_item, uint8_t state, uint8_t flags)
{
    char CURRS[5] = "func\0";     // current section to write
    if (!state) 
        ARR_WRITE_STR(CURRS, ((char*)"code\0"), 5);

    char*operand = NULL;
    bool was_condition = false;

    for (int i = 0; i < self_item->ast->list->po; i++)
    {
        ListItem*operand_item = self_item->ast->list->items[i];

        // get the operand after statement
        if (operand == NULL && i != 0 && i % 2 == 0)
        {
            ListItem*item = self_item->ast->list->items[i - 1];
            
            switch (item->ast->type) {
                case AST_OPEN_BRACKET:              // (
                case AST_CLOSE_BRACKET:             // )
                case AST_OP_ADD:                    // +
                case AST_OP_SUB:                    // -
                case AST_OP_MULT:                   // *
                case AST_OP_DIV:                    // /
                case AST_OP_AMPER:                  // &
                case AST_OP_VB:                     // |
                case AST_OP_REM_OF_DIV:             // %
                case AST_OP_NOT:                    // !
                case AST_OP_MORE:                   // >
                case AST_OP_LESS:                   // <
                case AST_OP_EQU:                    // =
                case AST_OP_ITEM_EQUAL:             // ==
                case AST_OP_ITEM_NOT_EQUAL:         // !=
                {
                    // todo
                    switch (item->ast->type) {
                        // case AST_OPEN_BRACKET:          { operand = ""; break; }
                        // case AST_CLOSE_BRACKET:         { operand = ""; break; }
                        case AST_OP_ADD:                { operand = "add"; break; }
                        case AST_OP_SUB:                { operand = "sub"; break; }
                        case AST_OP_MULT:               { operand = "imul"; break; }
                        case AST_OP_DIV:                { operand = "sdiv"; break; }
                        case AST_OP_AMPER:              { operand = "and"; break; }
                        case AST_OP_VB:                 { operand = "or"; break; }
                        // case AST_OP_REM_OF_DIV:         { operand = ""; break; }
                        // case AST_OP_MORE:               { operand = ""; break; }
                        // case AST_OP_LESS:               { operand = ""; break; }
                        // case AST_OP_EQU:                { operand = ""; break; }
                        case AST_OP_ITEM_EQUAL:         { operand = "cmp";  was_condition = true; break; }
                        case AST_OP_ITEM_NOT_EQUAL:     { operand = "ncmp"; was_condition = true; break; }
                    }

                } break;

                default: {
                    printE("There is must be math operation after statement!");
                    exit(1);
                } break;
            }
        }

        char*register_to_write = "eax";

        if (operand != NULL) {
            register_to_write = "ebx";
        }

        // if we got a type of a variable or function call,
        // we should get their values and store into register.
        // Actually, this also should be done for an int.
        // --------------------------------------------
        // if we have either plus or minus sign,
        // we have to add this to the next token as well.
        load_expression_item(parent_item, self_item, operand_item, register_to_write, CURRS, state, flags);

        switch (operand_item->ast->type) {
            case AST_OP_ADD:
            case AST_OP_SUB:
            case AST_OP_NOT: {
                uint64_t cnt = 0;
                
                Map operations;
                operations.fields = NULL;
                operations.size = 0;

                while (operand_item->ast->type == AST_OP_ADD ||
                        operand_item->ast->type == AST_OP_SUB ||
                        operand_item->ast->type == AST_OP_NOT) {
                    uint16_t type = operand_item->ast->type;
                    map_push(&operations, createIntIndexField(cnt++, (void*)(type)));

                    if (i < self_item->ast->list->po - 1)
                    {
                        operand_item = self_item->ast->list->items[++i];
                    }
                    else {
                        printE("There is must be something after not and negative operations!");
                        exit(1);
                    }
                }

                ListItem*item = self_item->ast->list->items[i];
                load_expression_item(parent_item, self_item, item, register_to_write, CURRS, state, flags);
                for (int j = operations.size - 1; j >= 0; --j) {
                    MapField_t*field_to_check = createIntIndexField(j, NULL);
                    MapField_t*field = map_get(&operations, field_to_check);

                    char*op;

                    switch ((int)field->value) {
                        case AST_OP_ADD:                { op = "nop"; break; }
                        case AST_OP_SUB:                { op = "neg"; break; }
                        case AST_OP_NOT:                { op = "lnot"; break; }
                    }

                    ADD_TO_SECTION(CURRS, "\t%s %s\n", op, register_to_write);

                    free((void*)field_to_check);
                    free((void*)field);
                }

                ++i;
            } break;
        }

        if (operand != NULL) {
            ADD_TO_SECTION(CURRS, "\t%s eax, ebx\n", operand);
            operand = NULL;
        }

    }

    if (flags & IF_FLAG && !was_condition)
    {
        ADD_TO_SECTION(CURRS, "\tncmp eax, 0\n", NULL);
    }

    return "eax";
}

void load_expression_item(ListItem*parent_item,
                          ListItem*self_item, 
                          ListItem*operand_item, 
                          char*register_to_write, 
                          char*CURRS, 
                          uint8_t state, 
                          uint8_t flags)
{
    switch (operand_item->ast->type) {
        case AST_STRING: {
                // save the string into data section
            char *hash = str_hash(format_string(operand_item->ast->name));
            char *old_ptr = hash;

            uint64_t int_hash = atoi(hash);

            if (hash[0] == '-') {
                hash[0] = 'M';
            }

            hash = concat("_HASH_", hash, NULL);
            MapField_t *field_to_check = createIntIndexField(int_hash, (void*)"_");

            if (map_get(&data_section_hashes, field_to_check) == NULL)
            {
                addDataSection(format_db_string(hash, operand_item->ast->name));
                map_push(&data_section_hashes, field_to_check);
            }
            else {
                free((void*)field_to_check);
            }

            ADD_TO_SECTION(CURRS, "\tmov %s, %s\n", register_to_write, hash); 

            free((void*)old_ptr);        
        } break;

        case AST_NUMBER: {
            ADD_TO_SECTION(CURRS, "\tmov %s, dword %d\n", register_to_write, operand_item->ast->number);            
        } break;

        case AST_CALL: {
            parseCall(parent_item, operand_item, state);
            ADD_TO_SECTION(CURRS, "\tmov %s, dword ecx\n", register_to_write, NULL);
        } break;

        case AST_VAR: {
            // get the value of the variable
            
            MapField_t *field_to_check = createStrIndexField(operand_item->ast->name, (void*)"_");
            MapField_t *result;

            bool global = false;

            result = map_get(parent_item->parent_local_variables, field_to_check);
            if (result == NULL)
                {
                    result = map_get(global_variables, field_to_check);
                    global = true;
                }

            if (result != NULL) {                        
                int64_t address = result->value;
                
                ADD_TO_SECTION(CURRS, "\tmov %s, dword [%s - %d]\n", register_to_write, (global ? "edi" : "ebp"), address);

            }
            else {
                printE("Unable to find variable '%s' in this scope!", operand_item->ast->name);
                exit(1);
            }

            free((void*)field_to_check);

        } break;
    }
}

void addCodeSection(const char*fmt, ...)
{
    char buffer[65536];
    memset(buffer, '\0', 65536);
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    code = concat(code, buffer, "");
}

void addDataSection(const char*fmt, ...)
{
    char buffer[65536];
    memset(buffer, '\0', 65536);
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);\
    data = concat(data, buffer, "");

}

void addFuncSection(const char*fmt, ...)
{
    char buffer[65536];
    memset(buffer, '\0', 65536);
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    funcs = concat(funcs, buffer, "");
}

void addExtrSection(const char*fmt, ...)
{
    char buffer[65536];
    memset(buffer, '\0', 65536);
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    extrns = concat(extrns, buffer, "");
}

void addHdrSection(const char*fmt, ...)
{
    char buffer[65536];
    memset(buffer, '\0', 65536);
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    header = concat(header, buffer, "");
}
