#pragma once

// slb - Salix bytecode

#include <cstdint>
#include "core.h"
#include "parser/AST.h"
#include "parser/Parser.h"

#define SLB_MAGIC 0x30112022

#define COMPILER_PUSH_VARIABLE(vec, ref_names, module, addr, type) { \
    for (const auto& a: ref_names) { printf(" - %s\n", a.c_str()); vec.push_back({ a, module, addr, type }); } }

#define COMPILER_CHECK_VARIABLE(vec, ref_names, mod) ((const char*)[&]{ \
    for (const auto& a: vec)                                            \
        for (const auto& b: ref_names){                                 \
            if (a.id == b && a.module == mod){                          \
                return b.c_str();}}                                     \
    return (const char*)nullptr; }())

namespace slx::bytecode {
    enum SlbInstruction {
        MULT = 0x00,
        DIV,
        ADD,
        SUB,
        JUMP,
        COND
    };

    struct SlbReference {
        std::string id;
        std::string module;

        uintptr_t address;
        parser::AST_Types type;
    };

    struct SlbOpcode {
        uint16_t instruction;
    };

    struct Slb {
        uint32_t magic;
        uint32_t version;

        size_t bytecodeSectionSize;
        size_t referencesSectionSize;

        SlbReference* references;
        SlbOpcode* bytecode;
    };

    void writeSlb(const std::string& path, Slb& slb);
    void validate(Slb& slb);

    class SlbCompiler {
    public:
        SlbCompiler(parser::Parser& par);
        ~SlbCompiler();

        Slb compile();

        void compileTree(std::vector<SlbReference> ref, parser::AST_Tree& tree, Slb& slb, uint64_t depth=0);

    private:
        parser::AST_Tree _tree;

    };
}