#include <utils.h>
#include <options.h>

#include <cmcode.h>
#include <list.h>
#include <lexer.h>
#include <parser.h>

int main(
        int argc, 
        char*argv[])
{
    uint64_t startCompilation = timeStamp();

    string   output_file    = "a.out";
    string   input_file     = NULL;
    uint8_t  cskip          = 0;

    for (int i = 1; i < argc; i++)
    {
        if (cskip--)
            continue;
        cskip = 1;

        switch (parseOption(argv[i]))
        {
            case _s:
                {
                    if (i + 1 >= argc)
                    {  printE("Option %s cannot be NULL!", argv[i]); exit(1);  }
                    input_file = argv[i + 1];
                    break;
                }

            case _o:
                 {
                    if (i + 1 >= argc)
                    {  printE("Option %s cannot be NULL!", argv[i]); exit(1);  }
                    output_file = argv[i + 1];
                    break;
                }
            
            case __silent:
                 {
                    _debug(1);
                    printI("Silent mode has been enabled");
                    break;
                 }

            default:
                {
                    printE("Invalid option %s!", argv[i]);
                    exit(1);
                    break;
                }
        }
    }
    
    if (input_file == NULL)
    {
        printE("Input file cannot be NULL! You have to specify it by using -s argument.");
        exit(1);
    }

    InputData*code = InputFile(input_file); 
    
    remove_comments(code);

    Lexer*lexer = createLexer(code);
    Parser*parser = createParser(lexer);
    AST*body = parserParseStatement(parser, true);
    
    char*assembly = initCmcode(createItem(body), false, true, true);
    // printf("\n%s\n", assembly);

    FILE*file;

    file = fopen("output.asm", "w+");
    fputs(assembly, file);
    fclose(file);

    system("fasm lang/lang.asm");
    system("fasm output.asm");
    system(concat("ld -m elf_i386 output.o lang/lang.o -o ", output_file, NULL));
    system(concat("chmod +x ", output_file, NULL));
    
    // system(concat("rm", output_file, NULL));
    // system(concat("gcc lang/precompiled.c inc.c -o ", output_file, NULL));
    // system("rm tmp.h");
    // system("rm inc.c");
    // system(concat("chmod +x ", output_file, NULL));

    printf("\n");
    printf("Compilation \033[92;1;4mcompleted\033[0m in %llu ms and saved as ./%s\n", timeStamp() - startCompilation, output_file);
}

