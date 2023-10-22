#pragma once

#include <llvm-c/Core.h>
#include <hashedbrown.h>
#include "token.h"
#include "shvType.h"

#define LTx127\
    ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"\
    ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

struct FileContext
{
    char *strings;
    LLVMContextRef context;
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    LLVMPassManagerRef pass;
    hashtable_T *identifiers;
};

struct ShvType htGetShvType(hashtable_T *ht, const char *key);

bool htSetShvType(hashtable_T *ht, const char *key, struct ShvType *val);

struct ShvType tokenToType(struct Token token, struct FileContext *ctx);

bool expectToken(int32_t actual, int32_t expected, struct FilePos *fpos, const char *fix);

bool compileLlvm(struct Token *tokens, char *strings);
