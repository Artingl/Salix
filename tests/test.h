#pragma once

#include <string>

struct Test {
    size_t id;

    bool shouldBeError;
    std::string stage;
    std::string compilerFlags;
    std::string code;
};
