#include "lexer/LexerFileParser.h"

#include <utility>
#include "io/fs.h"


namespace slx::lexer {
    LexerFileParser::LexerFileParser(std::vector<lexer::LexerToken>& tokens, size_t poolId, size_t depth, std::string module)
        : _depth(depth), _contents(core::getFilesPool()[poolId]), _tokens(tokens), _module(std::move(module)) {
    }

    LexerFileParser::~LexerFileParser() {
    }

    void LexerFileParser::parse() {
        std::string str;
        char startQuote;
        char symbol = _contents.value[0];
        bool privateUse = false;
        bool commentedLine = false;

        LexerTokenType multipleSignType;

        size_t lineNumber = 1;
        size_t lineOffset = 1;

        for (size_t location = 0; location < _contents.value.size(); symbol = _contents.value[++location]) {
            if (symbol == '\n') {
                lineNumber++;
                lineOffset = 1;
                commentedLine = false;
                continue;
            }

            ++lineOffset;

            if (symbol == ' ' || symbol == '\t' || commentedLine)
                continue;

            switch ((LexerTokenType)symbol) {
                case PLUS_SIGN:
                case MINUS_SIGN:
                case EQUALS_SIGN:
                case MORE_SIGN:
                case DIVIDE_SIGN:
                case MULTIPLY_SIGN:
                case LESS_SIGN:
                case AND:
                case OR:
                case PERCENT_SIGN:
                    multipleSignType = multipleSign((LexerTokenType) symbol,
                                                    (LexerTokenType) _contents.value[location + 1],
                                                    (LexerTokenType) _contents.value[location + 2]);

                    if (multipleSignType == COMMENT) {
                        commentedLine = true;
                        continue;
                    }

                    switch (multipleSignType) {
                        case FUNCTION_TYPE:
                        case DOUBLE_PLUS:
                        case DOUBLE_MINUS:
                        case DOUBLE_MORE_OR_EQUAL:
                        case DOUBLE_LESS_OR_EQUAL:
                        case CONDITION_EQUALS:
                        case AND:
                        case OR:
                        case LEFT_SHIFT:
                        case RIGHT_SHIFT:
                        case PLUS_EQUALS:
                        case MINUS_EQUALS:
                        case MULTIPLY_EQUALS:
                        case DIVIDE_EQUALS:
                        case AND_EQUALS:
                        case OR_EQUALS:
                        case EUCLIDEAN_DIVISION_EQUALS:
                            lineOffset++;
                            location++;
                            break;

                        case RIGHT_SHIFT_EQUALS:
                        case LEFT_SHIFT_EQUALS:
                            lineOffset += 2;
                            location += 2;
                    }

                    _tokens.push_back({
                        multipleSignType,
                        std::string(1, symbol),
                        _module,
                        _contents.path + _contents.fileName,
                        _contents.id,
                        lineNumber,
                        lineOffset,
                        _depth,
                        LEXER_TOKEN_MAGIC
                    });
                    break;

                case OPEN_BRACKET:
                case CLOSE_BRACKET:
                case OPEN_BRACE:
                case CLOSE_BRACE:
                case OPEN_SQUARE_BRACKET:
                case CLOSE_SQUARE_BRACKET:
                case AMPERSAND_SIGN:
                case VERTICAL_BAR_SIGN:
                case NOT_SIGN:
                case SEMICOLON_SIGN:
                case COMMA:
                case DOT:
                case COLON_SIGN:
                    _tokens.push_back({
                        (LexerTokenType)symbol,
                        std::string(1, symbol),
                        _module,
                        _contents.path + _contents.fileName,
                        _contents.id,
                        lineNumber,
                        lineOffset,
                        _depth,
                        LEXER_TOKEN_MAGIC
                    });
                    break;

                case QUOTE:
                case DOUBLE_QUOTE:
                    startQuote = symbol;
                    symbol = _contents.value[++location];

                    while (startQuote != symbol) {
                        str += symbol;
                        symbol = _contents.value[++location];
                    }

                    lineOffset += str.size() + 2;

                    _tokens.push_back({
                        STRING,
                        str,
                        _module,
                        _contents.path + _contents.fileName,
                        _contents.id,
                        lineNumber,
                        lineOffset,
                        _depth,
                        LEXER_TOKEN_MAGIC
                    });
                    break;

                default:
                    break;
            }

            if (std::isalpha(symbol) || symbol == '_') {
                LexerTokenType type = NONE;
                std::string alpha = _parseAlphaDigit(&location, false);

                if (alpha == "use") {
                    _skipSpaces(&location, &lineNumber, &lineOffset);

                    privateUse = false;
                    if (_contents.value[location] == '!') {
                        privateUse = true;
                        ++lineOffset;
                        ++location;
                        _skipSpaces(&location, &lineNumber, &lineOffset);
                    }

                    std::string result = _parseAlphaDigit(&location, false);
                    std::string fileName;

                    if (result == "std") fileName = "../std/" + result + ".slx";
                    else fileName = _contents.path + '/' + result + ".slx";

                    LexerFileParser(_tokens, core::addFileToPool(io::readfile(fileName), fileName), _depth+1, result).parse();
                }
                else if (alpha == "true") {
                    type = TRUE;
                }
                else if (alpha == "false") {
                    type = FALSE;
                }
                else {
                    type = ID;
                }

                if (type != NONE) {
                    lineOffset += alpha.size();

                    _tokens.push_back({
                        type,
                        alpha,
                        _module,
                        _contents.path + _contents.fileName,
                        _contents.id,
                        lineNumber,
                        lineOffset,
                        _depth,
                        LEXER_TOKEN_MAGIC
                    });
                }

                location--;
            }
            else if (std::isdigit(symbol)) {
                std::string number = _parseAlphaDigit(&location, true);
                lineOffset += number.size()-1;

                _tokens.push_back({
                    (number.find('.') != std::string::npos ? FLOAT : INTEGER),
                    number,
                    _module,
                    _contents.path + _contents.fileName,
                    _contents.id,
                    lineNumber,
                    lineOffset,
                    _depth,
                    LEXER_TOKEN_MAGIC
                });

                location--;
            }
        }
    }

    std::string LexerFileParser::_parseAlphaDigit(size_t* location, uint8_t type) {
        std::string result;
        bool wasDot = false;

        switch (type) {
            case 1:
                while (std::isdigit(_contents.value[(*location)]) ||
                       _contents.value[(*location)] == '_' ||
                       _contents.value[(*location)] == '.') {
                    if (_contents.value[(*location)] == '_') {
                        (*location)++;
                        continue;
                    }

                    if (_contents.value[(*location)] == '.') {
                        if (wasDot) {
                            io::debug_print(io::CRITICAL, "Unexpected dot sign in float value in file %s", (_contents.path + _contents.fileName).c_str());
                            exit(1);
                        }

                        wasDot = true;
                    }

                    result += _contents.value[(*location)++];
                }
                break;

            case 0:
                while (std::isalpha(_contents.value[(*location)]) ||
                        std::isdigit(_contents.value[(*location)]) ||
                        _contents.value[(*location)] == '_') {
                    result += _contents.value[(*location)++];
                }
                break;

            default:
                break;
        }

        return result;
    }

    void LexerFileParser::_skipSpaces(size_t *location, size_t* lineNumber, size_t* lineOffset) {
        for (; _contents.value[*location] == ' ' || _contents.value[*location] == '\t' || _contents.value[*location] == '\n'; (*location)++) {
            if (_contents.value[*location] == ' ' || _contents.value[*location] == '\t')
                (*lineOffset) += 1;

            if (_contents.value[*location] == '\n') {
                (*lineOffset)  = 0;
                (*lineNumber) += 1;
            }
        }
    }

    LexerTokenType LexerFileParser::multipleSign(LexerTokenType symbol, LexerTokenType nextSymbol, LexerTokenType thirdSymbol) {
        LexerTokenType type;

        switch (symbol) {
            case '-':
                type = nextSymbol == '>' ? FUNCTION_TYPE : nextSymbol == '-' ? DOUBLE_MINUS : nextSymbol == '=' ? MINUS_EQUALS : symbol;
                break;

            case '+':
                type = nextSymbol == '+' ? DOUBLE_PLUS : nextSymbol == '=' ? PLUS_EQUALS : symbol;
                break;

            case '<':
                type = nextSymbol == '=' ? DOUBLE_MORE_OR_EQUAL : nextSymbol == '<' ? thirdSymbol == '=' ? LEFT_SHIFT_EQUALS : LEFT_SHIFT : symbol;
                break;

            case '>':
                type = nextSymbol == '=' ? DOUBLE_LESS_OR_EQUAL : nextSymbol == '>' ? thirdSymbol == '=' ? RIGHT_SHIFT_EQUALS : RIGHT_SHIFT : symbol;
                break;

            case '=':
                type = nextSymbol == '=' ? CONDITION_EQUALS : symbol;
                break;

            case '&':
                type = nextSymbol == '&' ? AND : nextSymbol == '=' ? AND_EQUALS : symbol;
                break;

            case '|':
                type = nextSymbol == '|' ? OR : nextSymbol == '=' ? OR_EQUALS : symbol;
                break;

            case '/':
                type = nextSymbol == '/' ? COMMENT : nextSymbol == '=' ? DIVIDE_EQUALS : symbol;
                break;

            case '*':
                type = nextSymbol == '=' ? MULTIPLY_EQUALS : symbol;
                break;

            case '%':
                type = nextSymbol == '=' ? EUCLIDEAN_DIVISION_EQUALS : symbol;
                break;

            default:
                type = (LexerTokenType)symbol;
        }

        return type;
    }


}
