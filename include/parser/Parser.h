#pragma once

#include "lexer/Lexer.h"
#include "parser/AST.h"

namespace slx::parser {
    class Parser {
    public:
        Parser(lexer::Lexer& lex);
        ~Parser();

        void createTree(AST_Tree& tree);

        // Models
        AST_ModelBase<AST_FunctionArgument> createArgumentsModel(AST_Tree& module);
        AST_ModelBase<AST_Condition> createConditionModel(lexer::TokenTypesList stopToken, AST_Tree& module);
        AST_ModelBase<AST_ModelBase<AST_Condition>> createCallArgumentsModel(AST_Tree &tree);

        lexer::LexerToken getItem(bool inc);
        AST_Tree createIdentifier(AST_Tree &module);
        std::vector<std::string> createName(AST_Tree &module);
        std::string createIdentifierString(AST_Tree tree);

        static lexer::LexerToken &verify(lexer::LexerToken &token, lexer::LexerTokenType type, AST_Tree &module);

    private:
        size_t _currentItem;
        std::vector<lexer::LexerToken> _tokens;
    };
}
