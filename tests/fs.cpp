#include <atomic>
#include "fs.h"
#include "Logger.h"

namespace io {

    void writeFile(const std::string& value, const std::string& path) {
        std::string content;
        FILE* f = fopen(path.c_str(), "w");
        fputs(value.c_str(), f);
        fclose(f);
    }

    std::string readfile(const std::string& path)
    {
        if (access(path.c_str(), F_OK) != 0) {
            io::debug_print(CRITICAL, "File %s does not exist!", path.c_str());
            exit(1);
        }

        std::string content;
        FILE* f = fopen(path.c_str(), "rb+");

        fseek(f, 0, SEEK_END);
        size_t fileSize = ftell(f);
        rewind(f);

        for (size_t i = 0; i < fileSize; i++)
            content += ' ';

        fread(&content[0], sizeof(uint8_t), fileSize, f);
        fclose(f);

        return content;
    }

    std::string dirnameOf(const std::string& fname)
    {
        size_t pos = fname.find_last_of("\\/");
        return (std::string::npos == pos)
               ? ""
               : fname.substr(0, pos);
    };

    std::string namepathOf(const std::string& fname)
    {
        size_t pos = fname.find_last_of("\\/");
        return (std::string::npos == pos)
               ? ""
               : fname.substr(pos);
    };

}
