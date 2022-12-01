#include "io/ArgumentsParsing.h"
#include "core.h"
#include "io/fs.h"

namespace slx::io {

    bool ArgumentsValues::hasFlag(ArgumentsFlags flag) {
        return std::count(_flags.begin(), _flags.end(), flag);
    }

    void ArgumentsValues::addFlag(ArgumentsFlags flag) {
        _flags.push_back(flag);
    }


    ArgumentsValues parseArguments(uint32_t argc, char** argv) {
        ArgumentsValues values;
        bool wasInputFile = false;

        for (size_t i = 1; i < argc; i++) {
            std::string argument(argv[i]);

            if (argument == "--help" || argument == "-h") {
                printf("Usage: %s [options] file...\n"
                       "Options:\n"
                       "  -i, -I            Input file.\n"
                       "  -O0               Disable optimizations.\n"
                       "  -O1               1st level of optimizations.\n"
                       "  -O2               2nd level of optimizations.\n"
                       "  -O3               Full level of optimizations.\n"
                       "--help, -h          Display this information.\n"
                       , argv[0]);
                exit(0);
            }
            else if (argument == "-i" || argument == "-I") {
                if (wasInputFile) {
                    io::debug_print(CRITICAL, "You've already specified input file!");
                    exit(1);
                }

                values.inputFilePath = argv[++i];
                values.inputFileContents = io::readfile(values.inputFilePath);
                wasInputFile = true;
                continue;
            }
            else if (!wasInputFile) {
                values.inputFilePath = argv[i];
                values.inputFileContents = io::readfile(values.inputFilePath);
                wasInputFile = true;
                continue;
            }
            else {
                io::debug_print(CRITICAL, "Invalid option %s!", argument.c_str());
                exit(1);
            }
        }

        if (!wasInputFile) {
            io::debug_print(CRITICAL, "You should provide input file as the first argument or using -I flag!");
            exit(1);
        }

        return values;
    }

}
