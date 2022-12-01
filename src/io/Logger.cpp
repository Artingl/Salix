#include "io/Logger.h"

#include <cstdarg>
#include <chrono>

#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>


namespace slx::io {
    void debug_print(Level type, const std::string message, ...) {
        struct winsize w;
        ioctl(fileno(stdout), TIOCGWINSZ, &w);
        int32_t size = w.ws_col <= 0 ? 80 : w.ws_col;

        const char * const zcFormat = message.c_str();
        char* logType = (char *)(type == Level::NOTICE ? "\033[37;1;4mNOTICE" : type == Level::INFO ? "\033[34;1;4mINFO" :
                                 type == Level::WARNING ? "\033[33;1;4mWARNING" : type == Level::ERROR ? "\033[31;1;4mERROR" : "\033[31;1;4mCRITICAL ERROR");

        printf("%s:\033[0m ", logType);

        va_list args;
        va_start(args, message);
        int32_t iLen = std::vprintf(zcFormat, args);
        va_end(args);

        printf("\n");
    }

    uint64_t timestamp() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
}
