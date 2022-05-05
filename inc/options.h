#pragma once

#include <utils.h>

#define undefined   0x0
#define _s          0x1     // path to the source
#define _o          0x2     // output file
#define __run       0x3     // compile and run file from -s option
#define __silent    0x4     // disable all errors and warning from the code

uint32_t parseOption(const char*fmt);

