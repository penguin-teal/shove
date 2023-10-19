#pragma once

#include <stdbool.h>
#include "ir.h"

struct Token *irDeclareFn(struct Token *tokens,
                 struct FileContext *ctx,
                 LLVMValueRef *fn,
                 LLVMTypeRef *fnType,
                 struct ShvType *returnTypeOut,
                 struct ShvType *paramTypesOut,
                 uint32_t *paramCountOut
);

struct Token *irDefineFn(LLVMValueRef fn,
                struct Token *tokens,
                char *strings,
                struct FileContext *ctx,
                hashtable_T *fnScope,
                struct ShvType returnType,
                struct ShvType *parameterTypes
);