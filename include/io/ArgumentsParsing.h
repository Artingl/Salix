#pragma once

#include "io/Logger.h"

#include <string>
#include <vector>
#include <algorithm>

namespace slx::io {
    enum ArgumentsFlags {
        O0, O1, O2, O3
    };

    struct ArgumentsValues {
        std::string inputFilePath;
        std::string inputFileContents;

        bool hasFlag(ArgumentsFlags flag);

        void addFlag(ArgumentsFlags flag);

    private:
        std::vector<ArgumentsFlags> _flags;

    };

    ArgumentsValues parseArguments(uint32_t argc, char** argv);

}
