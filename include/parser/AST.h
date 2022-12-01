#pragma once

#include <vector>
#include <any>
#include <map>

namespace slx::parser {

    enum AST_Types {
        UNDEFINED,

        BASE, FUNCTION, RETURN, FUNCTION_CALL, VARIABLE, IF, ELSE, STATEMENT, VARIABLE_SET
    };

    enum AST_ConditionalTypes {
        NONE,

        AND_CONDITION,
        OR_CONDITION,
        NOT_CONDITION,
        FLOAT,
        INTEGER,
        STRING,
        TRUE,
        FALSE,

        LEFT_SHIFT,
        RIGHT_SHIFT,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        EUCLIDEAN_DIVISION,
        OR,
        AND,
        DOUBLE_PLUS,
        DOUBLE_MINUS,
        DOUBLE_MORE_OR_EQUAL,
        DOUBLE_LESS_OR_EQUAL,
        CONDITION_EQUALS,

        CALL,
        VAR
    };

    struct AST_FunctionArgument {
        std::vector<std::string> names;
        std::string dataType;
    };

    template<typename T>
    struct AST_ModelBase {
        std::vector<T> arguments;
    };

    struct AST_Tree {
        AST_Types type;

        std::vector<AST_Tree> items;
        std::vector<std::string> names;
        std::string value;
        std::string filePath;
        std::string module;
        std::string dataType;
        std::any model;

        size_t lineNumber;
        size_t lineOffset;
    };

    struct AST_Condition {
        AST_ConditionalTypes type;
        std::string value;
        std::string dataType;
        AST_Tree tree;
    };

    std::string getLocation(const parser::AST_Tree &tree);
    std::string getTypeName(AST_Types type);
}
