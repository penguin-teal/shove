#pragma once

#include <llvm-c/Core.h>
#include "ir.h"
#include "token.h"
#include "shvType.h"

struct Token *compileExpr(LLVMValueRef *result,
                          struct ShvType typeExpectation,
                          struct Token *tokens,
                          char *strings,
                          struct FileContext *ctx
);