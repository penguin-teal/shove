#pragma once

#include <stdint.h>

struct FilePos
{
    const char *fileName;
    uint32_t line;
    uint32_t col;
};

enum ShvError
{
    MAX_PARAMS = 0x0001,
    MISMATCH_SHOVE = 0x0002,
    UNEXPECTED_TOKEN = 0x0003
};