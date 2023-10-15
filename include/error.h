#pragma once

#include <stdint.h>

struct FilePos
{
    const char *fileName;
    uint32_t line;
    uint32_t col;
};
