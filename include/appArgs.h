#pragma once

#include <stdbool.h>

struct AppArgs
{
    char *args[1];
    bool verbose;
    bool quiet;
    char *outFile;
    char *err;
    char *warn;
    bool pedantic;
    bool streetRules;
};

bool doArgp(struct AppArgs *appArgs, int argc, char **argv);