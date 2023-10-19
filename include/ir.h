#pragma once

#include <llvm-c/Core.h>
#include <hashedbrown.h>
#include "token.h"

#define LTx127\
    ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"\
    ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

struct FileContext
{
    char *strings;
    LLVMContextRef context;
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    hashtable_T *identifiers;
};

struct ShvType tokenToType(struct Token token, struct FileContext *ctx);

bool expectToken(int32_t actual, int32_t expected, struct FilePos *fpos, const char *fix);

bool compileLlvm(struct Token *tokens, char *strings);
