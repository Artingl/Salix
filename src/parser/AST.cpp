#include <string>
#include "parser/AST.h"


namespace slx::parser {

    std::string getLocation(const parser::AST_Tree &tree) {
        return "line " + std::to_string(tree.lineNumber) + ", position " + std::to_string(tree.lineOffset) + " in module " + tree.module;
    }

    std::string getTypeName(AST_Types type) {

        std::string value = "NULL";

        switch (type) {
            case UNDEFINED:
                value = "undefined";
                break;

            case BASE:
                value = "base";
                break;

            case FUNCTION:
                value = "function statement";
                break;

            case RETURN:
                value = "return";
                break;

            case FUNCTION_CALL:
                value = "function call";
                break;

            case VARIABLE:
                value = "variable";
                break;

            case IF:
                value = "if statement";
                break;

            case ELSE:
                value = "else statement";
                break;

            case STATEMENT:
                value = "statement";
                break;

            case VARIABLE_SET:
                value = "variable set statement";
        }

        return value;
    }

}
