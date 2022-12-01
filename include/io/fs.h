#pragma once

#include "io/Logger.h"

#include <string>
#include <vector>

namespace slx::io {

    void writefile(const std::string& path, const std::string& data);
    void writefile(const std::string& path, uint8_t* data, size_t size);
    void writefile(const std::string& path, const std::vector<uint8_t>& data);

    std::string readfile(const std::string& path);
    std::string dirnameOf(const std::string& fname);
    std::string namepathOf(const std::string& fname);

}
