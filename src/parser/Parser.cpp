#include <algorithm>

#include "parser/Parser.h"
#include "io/Logger.h"

namespace slx::parser {

    Parser::Parser(lexer::Lexer& lex)
        : _tokens(lex.parse()) {
        _currentItem = 0;
    }

    Parser::~Parser() {
    }

    lexer::LexerToken & Parser::verify(lexer::LexerToken &token, lexer::LexerTokenType type, AST_Tree& module) {
        if (token.type != type) {
            io::debug_print(io::CRITICAL, "Unexpected value '%s'! Expected '%s' at %s",
                            lexer::getLexerTypeName(token.type).c_str(),
                            lexer::getLexerTypeName(type).c_str(),
                            (token.magic != LEXER_TOKEN_MAGIC ? parser::getLocation(module) : lexer::getLocation(token)).c_str());
            exit(1);
        }

        return token;
    }

    void Parser::createTree(AST_Tree& tree) {
        lexer::LexerToken a;
        bool wasOpened = false;

        if (_currentItem >= _tokens.size()-1) {
            io::debug_print(io::CRITICAL, "Unexpected end of file! at %s",
                            parser::getLocation(tree).c_str());
            exit(1);
        }

        if (_tokens[_currentItem].type == lexer::OPEN_BRACE)
        {
            _currentItem++;
            wasOpened = true;
        }

        for (;_currentItem < _tokens.size(); ++_currentItem) {
            a = _tokens[_currentItem];

            if (wasOpened && a.type == lexer::CLOSE_BRACE) {
                break;
            }

            if (a.magic != LEXER_TOKEN_MAGIC)
                continue;

            AST_Tree itemTree;
            itemTree.type = UNDEFINED;

            switch (a.type) {
                case lexer::ID:
                    if (a.value != "fn" && tree.type == BASE) {
                        io::debug_print(io::CRITICAL, "Unexpected identifier '%s'. Expected 'fn' at %s",
                                        a.value.c_str(),
                                        lexer::getLocation(a).c_str());
                        exit(1);
                    }

                    if (a.value == "fn") {
                        ++_currentItem;

                        itemTree.names = createName(tree);

                        itemTree.lineOffset = _tokens[_currentItem].lineOffset;
                        itemTree.lineNumber = _tokens[_currentItem].lineNumber;

                        verify(_tokens[_currentItem++], lexer::OPEN_BRACKET, tree);
                        itemTree.model = std::make_any<AST_ModelBase<AST_FunctionArgument>>(createArgumentsModel(tree));
                        verify(_tokens[_currentItem++], lexer::CLOSE_BRACKET, tree);

                        if (_tokens[_currentItem].type == lexer::ID) {
                            itemTree.value = _tokens[_currentItem].value;
                            ++_currentItem;
                        }

                        verify(_tokens[_currentItem++], lexer::FUNCTION_TYPE, tree);

                        itemTree.dataType = verify(_tokens[_currentItem++], lexer::ID, tree).value;
                        itemTree.type = FUNCTION;
                        itemTree.module = a.module;
                        itemTree.filePath = a.filePath;

                        createTree(itemTree);
                        tree.items.push_back(itemTree);
                    }
                    else if (a.value == "let") {
                        ++_currentItem;

                        itemTree.lineOffset = _tokens[_currentItem].lineOffset;
                        itemTree.lineNumber = _tokens[_currentItem].lineNumber;

                        itemTree.names = createName(tree);
                        verify(_tokens[_currentItem++], lexer::COLON_SIGN, tree);

                        itemTree.dataType = verify(_tokens[_currentItem++], lexer::ID, tree).value;
                        itemTree.type = VARIABLE;
                        itemTree.module = a.module;
                        itemTree.filePath = a.filePath;

                        if (_tokens[_currentItem].type == lexer::EQUALS_SIGN) {
                            verify(_tokens[_currentItem++], lexer::EQUALS_SIGN, tree);
                            itemTree.model = std::make_any<AST_ModelBase<AST_Condition>>(
                                    createConditionModel({ lexer::SEMICOLON_SIGN }, tree));
                        }
                        else if (_tokens[_currentItem].type != lexer::SEMICOLON_SIGN) {
                            io::debug_print(io::CRITICAL, "Unexpected token '%s'! Expected either ';' or '=' at %s",
                                            lexer::getLexerTypeName(a.type).c_str(),
                                            lexer::getLocation(a).c_str());
                            exit(1);
                        }

                        verify(_tokens[_currentItem], lexer::SEMICOLON_SIGN, tree);
                        tree.items.push_back(itemTree);
                    }
                    else if (a.value == "if") {
                        ++_currentItem;

                        itemTree.lineOffset = _tokens[_currentItem].lineOffset;
                        itemTree.lineNumber = _tokens[_currentItem].lineNumber;

                        verify(_tokens[_currentItem++], lexer::OPEN_BRACKET, tree);
                        itemTree.model = std::make_any<AST_ModelBase<AST_Condition>>(
                                createConditionModel({ lexer::CLOSE_BRACKET }, tree));
                        verify(_tokens[_currentItem++], lexer::CLOSE_BRACKET, tree);

                        itemTree.type = IF;
                        itemTree.module = a.module;
                        itemTree.filePath = a.filePath;

                        createTree(itemTree);
                        tree.items.push_back(itemTree);
                    }
                    else if (a.value == "else") {
                        ++_currentItem;

                        itemTree.lineOffset = _tokens[_currentItem].lineOffset;
                        itemTree.lineNumber = _tokens[_currentItem].lineNumber;

                        itemTree.type = ELSE;
                        itemTree.module = a.module;
                        itemTree.filePath = a.filePath;

                        createTree(itemTree);
                        tree.items.push_back(itemTree);
                    }
                    else if (a.value == "return") {
                        ++_currentItem;

                        itemTree.lineOffset = _tokens[_currentItem].lineOffset;
                        itemTree.lineNumber = _tokens[_currentItem].lineNumber;

                        itemTree.type = RETURN;
                        itemTree.model = std::make_any<AST_ModelBase<AST_Condition>>(
                                createConditionModel({ lexer::SEMICOLON_SIGN }, tree));

                        verify(_tokens[_currentItem], lexer::SEMICOLON_SIGN, tree);
                        tree.items.push_back(itemTree);
                    }
                    else if (_tokens[_currentItem+1].type == lexer::PLUS_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::MINUS_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::MULTIPLY_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::DIVIDE_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::EUCLIDEAN_DIVISION_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::AND_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::OR_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::RIGHT_SHIFT_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::LEFT_SHIFT_EQUALS ||
                            _tokens[_currentItem+1].type == lexer::EQUALS_SIGN) {
                        AST_Tree id = createIdentifier(tree);
                        lexer::LexerTokenType token = _tokens[_currentItem].type;

                        if (id.type != VARIABLE) {
                            io::debug_print(io::CRITICAL, "Setting value of '%s' is not allowed! at %s",
                                            parser::getTypeName(id.type).c_str(),
                                            lexer::getLocation(a).c_str());
                            exit(1);
                        }
                        ++_currentItem;

                        itemTree.lineOffset = _tokens[_currentItem].lineOffset;
                        itemTree.lineNumber = _tokens[_currentItem].lineNumber;

                        itemTree.type = VARIABLE_SET;
                        itemTree.module = a.module;
                        itemTree.filePath = a.filePath;
                        itemTree.value = id.value;

                        AST_ModelBase<AST_Condition> model = createConditionModel({ lexer::SEMICOLON_SIGN }, tree);

                        if (token != lexer::EQUALS_SIGN) {
                            switch (token) {
                                case lexer::PLUS_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {PLUS, "+"});
                                    break;
                                case lexer::MINUS_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {MINUS, "-"});
                                    break;
                                case lexer::MULTIPLY_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {MULTIPLY, "*"});
                                    break;
                                case lexer::DIVIDE_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {DIVIDE, "/"});
                                    break;
                                case lexer::EUCLIDEAN_DIVISION_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {EUCLIDEAN_DIVISION, "%"});
                                    break;
                                case lexer::AND_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {AND, "&"});
                                    break;
                                case lexer::OR_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {OR, "|"});
                                    break;
                                case lexer::RIGHT_SHIFT_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {RIGHT_SHIFT, ">>"});
                                    break;
                                case lexer::LEFT_SHIFT_EQUALS:
                                    model.arguments.insert(model.arguments.begin(), {LEFT_SHIFT, "<<"});
                                    break;
                            }

                            model.arguments.insert(model.arguments.begin(), { VAR, id.value, "", id });
                        }

                        itemTree.model = std::make_any<AST_ModelBase<AST_Condition>>(model);

                        verify(_tokens[_currentItem], lexer::SEMICOLON_SIGN, tree);
                        tree.items.push_back(itemTree);
                    }

                    if (itemTree.type != UNDEFINED)
                        break;
                case lexer::INTEGER:
                case lexer::FLOAT:
                case lexer::STRING:
                case lexer::DOUBLE_PLUS:
                case lexer::DOUBLE_MINUS:
                case lexer::TRUE:
                case lexer::FALSE:
                case lexer::NOT_SIGN:
                case lexer::OPEN_BRACKET:
                    itemTree.lineOffset = _tokens[_currentItem].lineOffset;
                    itemTree.lineNumber = _tokens[_currentItem].lineNumber;

                    itemTree.type = STATEMENT;
                    itemTree.module = a.module;
                    itemTree.filePath = a.filePath;

                    itemTree.model = std::make_any<AST_ModelBase<AST_Condition>>(
                            createConditionModel({ lexer::SEMICOLON_SIGN }, tree));
                    verify(_tokens[_currentItem], lexer::SEMICOLON_SIGN, tree);
                    tree.items.push_back(itemTree);
                    break;

                case lexer::OPEN_BRACE:
                    itemTree.lineOffset = _tokens[_currentItem].lineOffset;
                    itemTree.lineNumber = _tokens[_currentItem].lineNumber;

                    itemTree.type = BASE;
                    itemTree.module = a.module;
                    itemTree.filePath = a.filePath;

                    createTree(itemTree);
                    tree.items.push_back(itemTree);
                    break;

                case lexer::SEMICOLON_SIGN:
                    break;

                default:
                    io::debug_print(io::CRITICAL, "Unexpected value '%s' at %s",
                                    lexer::getLexerTypeName(a.type).c_str(),
                                    lexer::getLocation(a).c_str());
                    exit(1);
            }

            if (tree.type == FUNCTION || tree.type == IF || tree.type == ELSE) {
                if (!wasOpened)
                    break;
            }
        }

        if (wasOpened)
        {
            lexer::LexerToken item = getItem(true);
            verify(item, lexer::CLOSE_BRACE, tree);
        }

        if (tree.type == BASE && _currentItem < _tokens.size()-1) {
            io::debug_print(io::CRITICAL, "Unexpected value '%s' at %s",
                            lexer::getLexerTypeName(a.type).c_str(),
                            lexer::getLocation(a).c_str());
            exit(1);
        }

        if (wasOpened)
            _currentItem--;
    }

    AST_ModelBase<AST_FunctionArgument> Parser::createArgumentsModel(AST_Tree& module) {
        AST_ModelBase<AST_FunctionArgument> model;
        size_t offset = 0;

        while (_tokens[_currentItem].type != lexer::CLOSE_BRACKET) {
            if (_tokens[_currentItem].type == lexer::CLOSE_BRACKET && offset++ == 0)
                break;

            std::vector<std::string> names = createName(module);
            verify(_tokens[_currentItem++], lexer::COLON_SIGN, module);
            std::string type = verify(_tokens[_currentItem++], lexer::ID, module).value;

            model.arguments.push_back({ names, type });

            if (_tokens[_currentItem].type == lexer::COMMA) {
                ++_currentItem;
            }
            else if (_tokens[_currentItem].type != lexer::CLOSE_BRACKET) {
                io::debug_print(io::CRITICAL, "Unexpected value '%s' at %s. Expected either ')' or ','",
                                lexer::getLexerTypeName(_tokens[_currentItem].type).c_str(),
                                lexer::getLocation(_tokens[_currentItem]).c_str());
                exit(1);
            }
        }

        return model;
    }

    AST_ModelBase<AST_Condition> Parser::createConditionModel(lexer::TokenTypesList stopToken, AST_Tree& module) {
        AST_Tree id;
        AST_ModelBase<AST_Condition> model;

        int64_t brackets = 0;
        bool onlyNumber = true;
        bool lastSign = false;

        while (_tokens[_currentItem].type != stopToken.type0 &&
               _tokens[_currentItem].type != stopToken.type1  &&
               _tokens[_currentItem].type != stopToken.type2  &&
               _tokens[_currentItem].type != stopToken.type3 || brackets != 0) {
            lexer::LexerToken token = _tokens[_currentItem];

            if (token.magic != LEXER_TOKEN_MAGIC) {
                io::debug_print(io::CRITICAL,
                                "Unexpected end of file! Maybe you forgot to close bracket in module %s",
                                module.module.c_str());
                exit(1);
            }

            switch (token.type) {
                case lexer::ID:
                    id = createIdentifier(module);
                    _currentItem--;

                    model.arguments.push_back({
                        id.type == VARIABLE ? VAR : CALL,
                        id.value, "", id
                    });
                    onlyNumber = false;
                    lastSign = false;
                    break;

                case lexer::STRING:
                    model.arguments.push_back({
                        STRING,
                        token.value
                    });
                    onlyNumber = false;
                    lastSign = false;
                    break;

                case lexer::OPEN_BRACKET:
                    brackets++;
                    lastSign = false;
                    break;

                case lexer::CLOSE_BRACKET:
                    brackets--;

                    if (brackets < 0) {
                        verify(token, lexer::OPEN_BRACKET, module);
                    }
                    lastSign = false;
                    break;

                case lexer::AND:
                    model.arguments.push_back({
                        AND_CONDITION,
                        "&&"
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::OR:
                    model.arguments.push_back({
                        OR_CONDITION,
                        "&&"
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::NOT_SIGN:
                    model.arguments.push_back({
                        NOT_CONDITION,
                        token.value
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::TRUE:
                    model.arguments.push_back({
                        TRUE,
                        "1",
                        "boolean"
                    });
                    lastSign = false;
                    break;

                case lexer::FALSE:
                    model.arguments.push_back({
                        FALSE,
                        "0",
                        "boolean"
                    });
                    lastSign = false;
                    break;

                case lexer::INTEGER:
                    model.arguments.push_back({
                        INTEGER,
                        token.value,
                        "int"
                    });
                    lastSign = false;
                    break;

                case lexer::FLOAT:
                    model.arguments.push_back({
                        FLOAT,
                        token.value,
                        "float"
                    });
                    lastSign = false;
                    break;

                case lexer::LEFT_SHIFT:
                    model.arguments.push_back({
                        LEFT_SHIFT,
                        "<<"
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::RIGHT_SHIFT:
                    model.arguments.push_back({
                        RIGHT_SHIFT,
                        ">>"
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::PLUS_SIGN:
                    model.arguments.push_back({
                        PLUS,
                        token.value
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::MINUS_SIGN:
                    model.arguments.push_back({
                        MINUS,
                        token.value
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::MULTIPLY_SIGN:
                    model.arguments.push_back({
                        MULTIPLY,
                        token.value
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::DIVIDE_SIGN:
                    model.arguments.push_back({
                        DIVIDE,
                        token.value
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::AMPERSAND_SIGN:
                    model.arguments.push_back({
                        AND,
                        token.value
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::PERCENT_SIGN:
                    model.arguments.push_back({
                        EUCLIDEAN_DIVISION,
                        token.value
                    });

                    if (lastSign)
                        goto unexpectedSign;
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::VERTICAL_BAR_SIGN:
                    model.arguments.push_back({
                        OR,
                        token.value
                    });
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::DOUBLE_PLUS:
                    model.arguments.push_back({
                        DOUBLE_PLUS,
                        token.value
                    });
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::DOUBLE_MINUS:
                    model.arguments.push_back({
                        DOUBLE_MINUS,
                        token.value
                    });
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::DOUBLE_MORE_OR_EQUAL:
                    model.arguments.push_back({
                        DOUBLE_MORE_OR_EQUAL,
                        token.value
                    });
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::DOUBLE_LESS_OR_EQUAL:
                    model.arguments.push_back({
                        DOUBLE_LESS_OR_EQUAL,
                        token.value
                    });
                    lastSign = true;
                    onlyNumber = false;
                    break;

                case lexer::CONDITION_EQUALS:
                    model.arguments.push_back({
                        CONDITION_EQUALS,
                        token.value
                    });
                    lastSign = true;
                    onlyNumber = false;
                    break;

                default:
                    io::debug_print(io::CRITICAL, "Unexpected value '%s' at %s",
                                    lexer::getLexerTypeName(token.type).c_str(),
                                    lexer::getLocation(token).c_str());
                    exit(1);
            }

            _currentItem++;
            continue;

unexpectedSign:
            io::debug_print(io::CRITICAL, "Unexpected value '%s' at %s",
                            lexer::getLexerTypeName(token.type).c_str(),
                            lexer::getLocation(token).c_str());
            exit(1);
        }

        // optimize number expressions (only if optimizations are enabled)
        // (e.g. the compiler will replace expressions like 10 + 10 as 20)
        //
        // todo: not working in most scenarios.
        if (onlyNumber && core::optimizations() > O0) {
            AST_Condition frontValue = model.arguments.front();
            AST_ConditionalTypes sign = NONE;
            AST_ConditionalTypes type = frontValue.type;

            double number = std::atof(frontValue.value.c_str());
            model.arguments.erase(model.arguments.begin());

            for (const auto& a: model.arguments) {
                if (a.type == FLOAT) {
                    type = FLOAT;
                }

                if (a.type == LEFT_SHIFT ||
                    a.type == RIGHT_SHIFT ||
                    a.type == PLUS ||
                    a.type == MINUS ||
                    a.type == MULTIPLY ||
                    a.type == DIVIDE ||
                    a.type == EUCLIDEAN_DIVISION ||
                    a.type == OR ||
                    a.type == AND) {
                    sign = a.type;
                }
                else {
                    if (sign != NONE)
                    {
                        switch (sign) {
                            case LEFT_SHIFT:
                                break;

                            case RIGHT_SHIFT:
                                break;

                            case PLUS:
                                number += std::atof(a.value.c_str());
                                break;

                            case MINUS:
                                number -= std::atof(a.value.c_str());
                                break;

                            case MULTIPLY:
                                number *= std::atof(a.value.c_str());
                                break;

                            case DIVIDE:
                                number /= std::atof(a.value.c_str());
                                break;

                            case EUCLIDEAN_DIVISION:
                                break;

                            case OR:
                                break;

                            case AND:
                                break;

                            default:
                                break;
                        }

                        sign = NONE;
                    }
                }
            }

            model.arguments.clear();
            model.arguments.push_back({
                type,
                (type == INTEGER ? std::to_string(number).substr(0,
                                                                 std::to_string(number).find('.')) : std::to_string(number)),
                type == INTEGER ? "int" : "float"
            });
        }

        if (lastSign) {
            io::debug_print(io::CRITICAL, "Unexpected value '%s' at %s",
                            lexer::getLexerTypeName(_tokens[_currentItem].type).c_str(),
                            lexer::getLocation(_tokens[_currentItem]).c_str());
            exit(1);
        }

        return model;
    }

    lexer::LexerToken Parser::getItem(bool inc) {
        if (_currentItem < _tokens.size()) {
            lexer::LexerToken item = _tokens[_currentItem];

            if (inc)
                _currentItem++;

            return item;
        }

        return {
            lexer::NONE,
            "",
            "",
            "",
            0,
            0,
            0,
            0,
            0
        };
    }

    AST_Tree Parser::createIdentifier(AST_Tree &module) {
        AST_Tree info;

        verify(_tokens[_currentItem], lexer::ID, module);

        info.lineOffset = _tokens[_currentItem].lineOffset;
        info.lineNumber = _tokens[_currentItem].lineNumber;
        info.value = createIdentifierString(module);

        if (_tokens[_currentItem].type == lexer::OPEN_BRACKET) {
            verify(_tokens[_currentItem++], lexer::OPEN_BRACKET, module);
            info.type = FUNCTION_CALL;
            info.model = std::make_any<AST_ModelBase<AST_ModelBase<AST_Condition>>>(
                    createCallArgumentsModel(module));
            verify(_tokens[_currentItem++], lexer::CLOSE_BRACKET, module);
        }
        else {
            info.type = VARIABLE;
        }

        return info;
    }

    AST_ModelBase<AST_ModelBase<AST_Condition>> Parser::createCallArgumentsModel(AST_Tree &tree) {
        AST_ModelBase<AST_ModelBase<AST_Condition>> arguments;

        while (_tokens[_currentItem].type != lexer::CLOSE_BRACKET) {
            arguments.arguments.push_back(createConditionModel({ lexer::COMMA, lexer::CLOSE_BRACKET }, tree));

            if (_tokens[_currentItem].type == lexer::CLOSE_BRACKET)
                break;

            verify(_tokens[_currentItem++], lexer::COMMA, tree);

            if (_tokens[_currentItem].type == lexer::CLOSE_BRACKET) {
                io::debug_print(io::CRITICAL, "Unexpected value '{'! Expected comma at %s",
                                lexer::getLocation(_tokens[_currentItem]).c_str());
                exit(1);
            }
        }

        return arguments;
    }

    std::vector<std::string> Parser::createName(AST_Tree &module) {
        std::vector<std::string> names;

        if (_tokens[_currentItem].type == lexer::ID) {
            names.push_back(createIdentifierString(module));
            return names;
        }
        else if (_tokens[_currentItem].type != lexer::OPEN_SQUARE_BRACKET) {
            io::debug_print(io::CRITICAL,
                            "Invalid name identifier %s! Expected either identifier or '[' at %s",
                            parser::getLocation(module).c_str());
            exit(1);
        }

        verify(_tokens[_currentItem++], lexer::OPEN_SQUARE_BRACKET, module);

        while (_tokens[_currentItem].type != lexer::CLOSE_SQUARE_BRACKET) {
            names.push_back(createIdentifierString(module));

            if (_tokens[_currentItem].type == lexer::COMMA) {
                ++_currentItem;
            }
            else if (_tokens[_currentItem].type != lexer::CLOSE_SQUARE_BRACKET) {
                io::debug_print(io::CRITICAL, "Unexpected value '%s' at %s. Expected either ']' or ','",
                                lexer::getLexerTypeName(_tokens[_currentItem].type).c_str(),
                                lexer::getLocation(_tokens[_currentItem]).c_str());
                exit(1);
            }
        }

        verify(_tokens[_currentItem++], lexer::CLOSE_SQUARE_BRACKET, module);

        return names;
    }

    std::string Parser::createIdentifierString(AST_Tree tree) {
        std::string result;

        while (_tokens[_currentItem].type == lexer::ID ||
               _tokens[_currentItem].type == lexer::DOT) {
            result += _tokens[_currentItem++].value;

            if (_tokens[_currentItem].type != lexer::DOT) {
                break;
            }
            else {
                result += _tokens[_currentItem++].value;
            }
        }

        return result;
    }

}
