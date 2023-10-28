#pragma once

#include <stdbool.h>
#include "ir.h"

/**
 * Does nothing or promotes bit size if needed, otherwise fails.
 */
bool implicitCast(
    struct FileContext *ctx,
    struct FilePos *fpos,
    struct ShvType *given,
    struct ShvType *target
);