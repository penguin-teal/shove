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
    next(&result2, &resT2, typeExpectation, atToken, strings, NULL, ctx);\
    } while(0);

struct ShvType divNums(
    LLVMValueRef *result,
    struct FileContext *ctx,
    struct ShvType *nTy,
    LLVMValueRef numer,
    struct ShvType *dTy,
    LLVMValueRef denom
)
{
    LLVMTypeKind nK = LLVMGetTypeKind(nTy->llvm);
    bool nS = nTy->isSigned;
    LLVMTypeKind dK = LLVMGetTypeKind(dTy->llvm);
    bool dS = dTy->isSigned;

    struct ShvType quotTy;
    if(nK == LLVMDoubleTypeKind || nK == LLVMFloatTypeKind
    || dK == LLVMDoubleTypeKind || dK == LLVMFloatTypeKind)
    {
        LLVMValueRef quot;
        quot = LLVMBuildFDiv(ctx->builder, numer, denom, "fdiv");
        quotTy.llvm = LLVMTypeOf(quot);
        quotTy.isSigned = true;
        quotTy.bitSize = LLVMGetTypeKind(quotTy.llvm) == LLVMDoubleTypeKind ? 64 : 32;
        *result = quot;
        return quotTy;
    }
    else
    {
        // If both unsigned
        if(nS == dS && !nS)
        {
            *result = LLVMBuildUDiv(ctx->builder, numer, denom, "udiv");
            quotTy.llvm = LLVMTypeOf(*result);
            quotTy.isSigned = false;
            quotTy.bitSize = LLVMGetIntTypeWidth(quotTy.llvm) * 8;
            return quotTy;
        }
        else
        {
            // Make both signed
            if(!nS)
            {
                numer = LLVMBuildIntCast2(ctx->builder, numer, nTy->llvm, true, "scast");
            }
            else if(!dS)
            {
                denom = LLVMBuildIntCast2(ctx->builder, denom, dTy->llvm, true, "ucast");
            }

            *result = LLVMBuildSDiv(ctx->builder, numer, denom, "sdiv");
            quotTy.llvm = LLVMTypeOf(*result);
            quotTy.isSigned = true;
            quotTy.bitSize = LLVMGetIntTypeWidth(quotTy.llvm) * 8;
            return quotTy;
        }
    }
}

static struct Token *next(LLVMValueRef *result,
                          struct ShvType *resultType,
                          struct ShvType typeExpectation,
                          struct Token *tokens,
                          char *strings,
                          bool *done,
                          struct FileContext *ctx
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
            // TODO
            *resultType = typeExpectation;
            return atToken;
        case TOKEN_MINUS:
            GET_NEXT();
            *result = LLVMBuildSub(
                ctx->builder,
                *result,
                result2,
                "sub"
            );
            // TODO
            *resultType = typeExpectation;
            return atToken;
        case TOKEN_STAR:
            GET_NEXT();
            *result = LLVMBuildMul(
                ctx->builder,
                *result,
                result2,
                "mul"
            );
            // TODO
            *resultType = typeExpectation;
            return atToken;
        case TOKEN_SLASH:
            GET_NEXT();
            *resultType = divNums(result, ctx, resultType, *result, &resT2, result2);
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
    struct ShvType resultType = { 0 };
    struct Token *atToken = tokens;
    while(true)
    {
        atToken = next(result, &resultType, typeExpectation,
            atToken, strings, &done, ctx);
        if(done) break;
    }

    return atToken;
}