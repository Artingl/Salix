#include "io/fs.h"
#include "lexer/LexerToken.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "bytecode/Slb.h"
#include "bytecode/SlbExec.h"
#include "core.h"

// todo for:
//  1. Write and interpreter/compiler to own byte code.
//  2. Make cycles, classes, enums, etc.
//  3. Being able to import (use) file privately (e.g. use! std),
//      so other modules wont be able to use imported modules from other files.

using namespace slx;

std::vector<core::PoolValue> _pool;
io::ArgumentsValues _argumentsValues;

int main(int argc, char** argv) {
    _argumentsValues = io::parseArguments(argc, argv);

    uint64_t startCompilation = io::timestamp();
    std::string output = _argumentsValues.inputFilePath + ".slb";

    lexer::Lexer lex(_argumentsValues.inputFileContents, _argumentsValues.inputFilePath);
    parser::Parser par(lex);
    bytecode::SlbCompiler compiler(par);
    bytecode::Slb slb = compiler.compile();
    bytecode::writeSlb(output, slb);

    printf("\n");
    printf("Compilation \033[92;1;4mcompleted\033[0m in %lu ms and saved as ./%s\n", io::timestamp() - startCompilation, output.c_str());

    return 0;
}


namespace slx::core {
    std::map<std::string, uint32_t> codes = {
            { "b0.1.3-a", 0x00000001 }
    };

    std::vector<PoolValue>& getFilesPool() {
        return _pool;
    }

    size_t addFileToPool(const std::string& value, const std::string& path) {
        _pool.push_back({ value, io::dirnameOf(path), io::namepathOf(path), _pool.size() });
        return _pool.size()-1;
    }

    uint8_t optimizations() {
        return O0;
    }

    uint32_t getVersionCode(const std::string& code) {
        return codes[code];
    }

    std::string getVersionFromCode(uint32_t v) {
        // todo
        for (const auto& a: codes) {
            if (a.second == v)
                return a.first;
        }

        return "NULL";
    }

    io::ArgumentsValues& getArguments() {
        return _argumentsValues;
    }
}
