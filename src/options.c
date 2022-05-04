#include <options.h>

uint32_t parseOption(const char*fmt)
{
    if (strcmp((const char*)"-s", fmt) == 0)
        return _s;
    
    if (strcmp((const char*)"-o", fmt) == 0)
        return _o;
    
    if (strcmp((const char*)"--run", fmt) == 0)
        return __run;

    if (strcmp((const char*)"--silent", fmt) == 0)
        return __silent;

    return undefined;
}


