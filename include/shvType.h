#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <llvm-c/Core.h>

struct ShvType
{
    LLVMTypeRef llvm;
    /**
     * Whether or not the type is signed, if relevant.
     */
    bool isSigned;
    uint64_t bitSize;
    char *friendly;
};

char *getShvTypeName(struct ShvType *t);

bool shvTypesAreCopies(struct ShvType *a, struct ShvType *b);
