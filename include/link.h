#pragma once

#include "list.h"

bool linkObjects(
    const char *outFName,
    string_list_T *objFNames,
    const char *linker,
    bool verbose
);