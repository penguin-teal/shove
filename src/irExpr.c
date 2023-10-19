#include <llvm-c/Core.h>
#include <stddef.h>
#include <stdint.h>
#include "ir.h"
#include "token.h"
#include "shvType.h"

struct Token *compileExpr(LLVMValueRef *result,
                          struct ShvType typeExpectation,
                          struct Token *tokens
)
{
    *result = NULL;
    struct Token *atToken = tokens;
    while(true)
    {
        switch(atToken->symbol)
        {
            case TOKEN_TERMINATE:
                // These will intentionally leave the next token as this token
                return atToken;
            case TOKEN_INTEGER:
                *result = LLVMConstInt(
                    typeExpectation.llvm,
                    atToken->value,
                    typeExpectation.isSigned
                );
                return atToken + 1;
        }
    }
}