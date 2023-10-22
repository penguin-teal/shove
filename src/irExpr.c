#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "defines.h"
#include "ir.h"
#include "irExpr.h"
#include "shvError.h"
#include "strOp.h"
#include "token.h"
#include "shvType.h"
#include "tokenUtil.h"
#include "irExpr.h"
#include "htblOp.h"

// to shorten next function cases
#define GET_NEXT() do { atToken++; atToken =\
    next(&result2, &resT2, typeExpectation, atToken, strings, NULL, ctx, (atToken - 1)->symbol);\
    } while(0);

static struct Token *next(LLVMValueRef *result,
                          struct ShvType *resultType,
                          struct ShvType typeExpectation,
                          struct Token *tokens,
                          char *strings,
                          bool *done,
                          struct FileContext *ctx,
                          int32_t lastToken
)
{
    struct Token *atToken = tokens;
    LLVMValueRef result2;
    struct ShvType resT2;

    switch(atToken->symbol)
    {
        case TOKEN_PAREN_CLOSE:
        case TOKEN_TERMINATE:
            // These will intentionally leave the next token as this token
            if(done) *done = true;
            return atToken;
        case TOKEN_INTEGER:
            *result = LLVMConstInt(
                typeExpectation.llvm,
                atToken->value,
                typeExpectation.isSigned
            );
            *resultType = typeExpectation;
            return atToken + 1;
        case TOKEN_PAREN_OPEN:
            atToken++;
            atToken = compileExpr(
                result,
                typeExpectation,
                atToken,
                strings,
                ctx
            );
            *resultType = typeExpectation;
            if(!expectToken(
                atToken->symbol,
                TOKEN_PAREN_CLOSE,
                &atToken->fpos,
                "Early semicolon, should be at end of statement."
            ))
            {
                if(done) *done = true;
                return NULL;
            }

            return atToken + 1;
        case TOKEN_FLOAT:
            {
                double fLiteral = 0;
                memcpy(&fLiteral, &atToken->value, sizeof(double));
                *result = LLVMConstReal(
                typeExpectation.llvm,
                    fLiteral
                );
                *resultType = typeExpectation;
                return atToken + 1;
            }
        case TOKEN_IDENTIFIER:
            {
                char *name = strings + atToken->value;
                struct ShvType t = htblGetType(ctx->identifiers, name);

                if(LLVMGetTypeKind(t.llvm) == LLVMFunctionTypeKind)
                {
                    NOT_IMPLEMENTED("Fn References");
                    return NULL;
                }
                else
                {
                    *result = LLVMBuildLoad2(
                        ctx->builder,
                        t.llvm,
                        htGetPtr(ctx->identifiers, name),
                        name
                    );
                }
                return atToken + 1;
            }
        case TOKEN_PLUS:
            GET_NEXT();
            *result = LLVMBuildAdd(
                ctx->builder,
                *result,
                result2,
                "add"
            );
            return atToken;
        case TOKEN_MINUS:
            GET_NEXT();
            *result = LLVMBuildSub(
                ctx->builder,
                *result,
                result2,
                "sub"
            );
            return atToken;
        default:
            shvIssue(
                SHVERROR_UNEXPECTED_TOKEN,
                &atToken->fpos,
                "Unexpected token.",
                "Unexpected token '%s' in expression.",
                getTokenString(atToken->symbol)
            );
            if(done) *done = true;
            return NULL;
    }
}

struct Token *compileExpr(LLVMValueRef *result,
                          struct ShvType typeExpectation,
                          struct Token *tokens,
                          char *strings,
                          struct FileContext *ctx
)
{
    bool done = false;
    *result = NULL;
    struct ShvType resultType;
    struct Token *atToken = tokens;
    int32_t lastToken = 0;
    while(true)
    {
        atToken = next(result, &resultType, typeExpectation,
            atToken, strings, &done, ctx, lastToken);
        if(done) break;
        else lastToken = atToken->symbol;
    }

    return atToken;
}