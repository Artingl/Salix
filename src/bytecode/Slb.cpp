#include "bytecode/Slb.h"
#include "io/Logger.h"
#include "io/fs.h"

#include "utils/Utils.h"

#include <cstring>

namespace slx::bytecode {

    void validate(Slb& slb) {
        if (slb.magic != SLB_MAGIC) {
            io::debug_print(io::CRITICAL, "Unable to parse Salix Bytecode file! Invalid magic.");
        }
        else if (core::getVersionCode(VERSION) < slb.version) {
            io::debug_print(io::CRITICAL,
                            "This version of Salix bytecode file was compiled using newer version of Salix. "
                            "Current version: %s, Slb version: %s",
                            VERSION, core::getVersionFromCode(slb.version).c_str());
        }
    }

    void writeSlb(const std::string& path, Slb& slb) {
        std::vector<uint8_t> data;
        EXTEND_VECTOR(data, sizeof(Slb))

        utils::writeImm32(data, slb.magic, offsetof(Slb, magic));
        utils::writeImm32(data, slb.version, offsetof(Slb, version));
        utils::writeImm32(data, slb.bytecodeSectionSize, offsetof(Slb, bytecodeSectionSize));
        utils::writeImm32(data, slb.referencesSectionSize, offsetof(Slb, referencesSectionSize));

        io::writefile(path, data);
    }

    SlbCompiler::SlbCompiler(parser::Parser& par){
        _tree.type = slx::parser::BASE;
        _tree.module = "main";
        _tree.filePath = core::getArguments().inputFilePath;
        _tree.lineOffset = 0;
        _tree.lineNumber = 0;

        par.createTree(_tree);
    }

    SlbCompiler::~SlbCompiler() {
    }

    Slb SlbCompiler::compile() {
        Slb slb { SLB_MAGIC, core::getVersionCode(VERSION), 0, 0 };
        std::vector<SlbReference> ref;
        compileTree(ref, _tree, slb);
        return slb;
    }

    void SlbCompiler::compileTree(std::vector<SlbReference> ref, parser::AST_Tree& tree, Slb& slb, uint64_t depth) {
        const char* dec;

        for (parser::AST_Tree& item: tree.items) {
            printf("MODULE:%s%s\t%s%s",
                   item.module.c_str(),
                   REPEAT_CHAR(' ', 10-item.module.size()).c_str(),
                   REPEAT_CHAR('-', depth*3).c_str(),
                   (depth>0?" ":""));

            switch (item.type) {
                case parser::IF:
                    printf("IF:\n");
                    compileTree(ref, item, slb, depth+1);
                    break;

                case parser::ELSE:
                    printf("ELSE:\n");
                    break;

                case parser::FUNCTION:
                    printf("FN %s:\n", item.names[0].c_str());

                    dec = COMPILER_CHECK_VARIABLE(ref, item.names, item.module);
                    assert_silent_critical(dec == nullptr,
                                           "Function '%s' has been already declared! at %s",
                                           dec, parser::getLocation(item).c_str())

                    COMPILER_PUSH_VARIABLE(ref, item.names, item.module, 0, item.type)

                    compileTree(ref, item, slb, depth+1);
                    break;

                case parser::RETURN:
                    printf("RETURN:\n");
                    break;

                case parser::FUNCTION_CALL:
                    printf("FUNCTION_CALL:\n");
                    break;

                case parser::VARIABLE:
                    printf("VARIABLE %s:\n", item.names[0].c_str());

                    dec = COMPILER_CHECK_VARIABLE(ref, item.names, item.module);
                    assert_silent_critical(dec == nullptr,
                           "Variable '%s' has been already declared! at %s",
                           dec, parser::getLocation(item).c_str())

                    COMPILER_PUSH_VARIABLE(ref, item.names, item.module, 0, item.type)
                    break;

                case parser::STATEMENT:
                    printf("STATEMENT:\n");
                    break;

                case parser::VARIABLE_SET:
                    printf("VARIABLE_SET %s:\n", item.value.c_str());
                    break;

                default:
                    break;
            }
        }

    }

}
