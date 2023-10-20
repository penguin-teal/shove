#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "hashedbrown.h"
#include "shvError.h"
#include "token.h"
#include "ir.h"
#include "tokenUtil.h"
#include "irExpr.h"
#include "shvType.h"

struct Token *irDeclareFn(struct Token *tokens,
                 struct FileContext *ctx,
                 LLVMValueRef *fn,
                 LLVMTypeRef *fnType,
                 struct ShvType *returnTypeOut,
                 struct ShvType *paramTypesOut,
                 uint32_t *paramCountOut
)
{
    struct Token ret = tokens[0];
    struct Token name = tokens[1];
    LLVMTypeRef parameterTypes[127];
    size_t parameterNames[127];

    uint32_t paramCount = 0;
    while(1)
    {
        struct Token shove = tokens[paramCount * 3 + 2];
        if(shove.symbol != TOKEN_SHOVE)
            break;

        struct Token paramType = tokens[paramCount * 3 + 3];
        if(!expectToken(paramType.symbol, TOKEN_IDENTIFIER, &paramType.fpos, "Follow shove by parameter type."))
            return false;

        struct Token paramName = tokens[paramCount * 3 + 4];
        if(!expectToken(paramName.symbol, TOKEN_IDENTIFIER, &paramType.fpos, "Follow type by parameter name."))
            return false;

        paramCount++;
        if(paramCount > 127)
        {
            shvIssue(
                SHVERROR_MAX_PARAMS,
                &shove.fpos,
                "Remove, like, a LOT of parameters.",
                "Maximum amount of parameters for a function is 127."
            );
            return NULL;
        }
        if(shove.value != paramCount)
        {
            char fixString[32 + 127];
            sprintf(fixString, "Shove should look like this: '-%.*s'.", paramCount, LTx127);

            if(shvIssue(
                SHVERROR_MISMATCH_SHOVE,
                &shove.fpos,
                fixString,
                "Incorrect shove depth, add another '>' every parameter."
            ))
                return NULL;
        }

        uint32_t inx = paramCount - 1;
        
        paramTypesOut[inx] = tokenToType(paramType, ctx);

        parameterNames[inx] = paramName.value;
        parameterTypes[inx] = paramTypesOut[inx].llvm;
    }

    *paramCountOut = paramCount;

    memset(
        paramTypesOut + paramCount,
        0,
        sizeof(struct ShvType)
    );

    *returnTypeOut = tokenToType(ret, ctx);

    LLVMTypeRef signature = LLVMFunctionType(
        returnTypeOut->llvm,
        parameterTypes, paramCount,
        false
    );

    LLVMValueRef fnVal = LLVMAddFunction(
            ctx->module,
            ctx->strings + name.value,
            signature
    );

    if(paramCount > 0)
    {
        LLVMValueRef firstParam = LLVMGetFirstParam(fnVal);
        LLVMValueRef paramN = firstParam;
        LLVMValueRef lastParam = LLVMGetLastParam(fnVal);
        size_t i = 0;
        while(true)
        {
            char *paramName = ctx->strings + parameterNames[i++];
            size_t paramNameLen = strlen(paramName);

            LLVMSetValueName2(
                paramN,
                paramName,
                paramNameLen
            );

            if(paramN == lastParam) break;
            paramN = LLVMGetNextParam(paramN);
        }
    }

    *fnType = signature;
    *fn = fnVal;
    return tokens + 2 + paramCount * 3;
}

struct Token *irDefineBranch(
    struct FileContext *ctx,
    hashtable_T *fnScope,
    struct ShvType returnType,
    struct Token *tokens,
    char *strings
)
{
    struct Token *tok = tokens;
    do
    {
        switch(tok->symbol)
        {
            case TOKEN_RETURN:
                tok++;
                if(LLVMGetTypeKind(returnType.llvm) == LLVMVoidTypeKind)
                {
                    LLVMBuildRetVoid(ctx->builder);
                }
                else
                {
                    LLVMValueRef result;
                    tok = compileExpr(
                        &result,
                        returnType,
                        tok
                    );
                    if(!tok) return NULL;

                    LLVMBuildRet(
                        ctx->builder,
                        result
                    );
                }

                if(!expectToken(tok->symbol,
                    TOKEN_TERMINATE,
                    &tok->fpos,
                    "Return statement missing semicolon."
                ))
                {
                    return NULL;
                }

                return tok + 1;
            default:
                shvIssue(
                    SHVERROR_UNEXPECTED_TOKEN,
                    &tok->fpos,
                    "Invalid token.",
                    "Unexpected token '%s'.",
                    getTokenString(tok->symbol)
                );
                return NULL;
        }
    }
    while(tok++);
}

struct Token *irDefineFn(LLVMValueRef fn,
                struct Token *tokens,
                char *strings,
                struct FileContext *ctx,
                hashtable_T *fnScope,
                struct ShvType returnType,
                struct ShvType *parameterTypes
)
{
    LLVMBasicBlockRef bb =
        LLVMAppendBasicBlockInContext(ctx->context, fn, "entry");
    
    LLVMPositionBuilder(ctx->builder, bb, NULL);

    htTableReset(fnScope);

    uint32_t paramCount = LLVMCountParams(fn);
    if(paramCount > 0)
    {
        LLVMValueRef paramN = LLVMGetFirstParam(fn);
        size_t whoAskedLength;
        uint32_t i = 0;

        do
        {
            htSetPtr(
                fnScope,
                LLVMGetValueName2(paramN, &whoAskedLength),
                paramN
            );
        }
        while(i < paramCount);
    }

    struct Token *atToken = tokens;
    if(!expectToken(
        atToken->symbol,
        TOKEN_BLOCK_OPEN,
        &atToken->fpos,
        "Put function body after declaration."
    )) return NULL;

    atToken++;
    atToken = irDefineBranch(ctx, fnScope, returnType, atToken, strings);
    
    if(!atToken)
    {
        return NULL;
    }
    else if(atToken->symbol != TOKEN_BLOCK_CLOSE)
    {
        if(
            !shvIssue(SHVERROR_UNREACHABLE_CODE, &atToken->fpos,
                "Remove unreachable code.",
                "Unreachable code. Expected '}'."
            )
        )
        {
            return NULL;
        }
    }

    return atToken + 1;
}