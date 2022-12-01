#pragma once

#include "Logger.h"

#include <string>

namespace io {

    void writeFile(const std::string& value, const std::string& path);

    std::string readfile(const std::string& path);
    std::string dirnameOf(const std::string& fname);
    std::string namepathOf(const std::string& fname);

}
