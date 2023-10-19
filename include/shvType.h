#pragma once

#include <stdbool.h>
#include <llvm-c/Core.h>

struct ShvType
{
    LLVMTypeRef llvm;
    /**
     * Whether or not the type is signed, if relevant.
     */
    bool isSigned;
};