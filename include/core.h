#pragma once

#include "io/ArgumentsParsing.h"

#include <string>
#include <vector>

#define O0 0        // No optimizations
#define O1 1
#define O2 2
#define O3 3

namespace slx::core {
    struct PoolValue {
        std::string value;
        std::string path;
        std::string fileName;
        size_t id;
    };

    std::vector<PoolValue> & getFilesPool();
    size_t addFileToPool(const std::string& value, const std::string& path);

    uint8_t optimizations();
    uint32_t getVersionCode(const std::string& code);

    std::string getVersionFromCode(uint32_t v);

    io::ArgumentsValues& getArguments();
}