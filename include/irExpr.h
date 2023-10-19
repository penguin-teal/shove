#pragma once

#include <llvm-c/Core.h>

struct Token *compileExpr(LLVMValueRef *result,
                          struct ShvType typeExpectation,
                          struct Token *tokens
);