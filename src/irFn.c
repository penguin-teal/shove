#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "hashedbrown.h"
#include "shvError.h"
#include "token.h"
#include "ir.h"
#include "tokenUtil.h"
#include "irExpr.h"
#include "shvType.h"
#include "strOp.h"

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

struct Token *statementStartsWithId(struct Token *tokens,
                                    char *strings,
                                    struct FileContext *ctx,
                                    LLVMBasicBlockRef firstBb
)
{
    struct Token *tok = tokens;
    switch((tok + 1)->symbol)
    {
        // Declaring ref variable
        case TOKEN_AMPERSAND:
            NOT_IMPLEMENTED("References");
            return NULL;
        // Declaring variable
        case TOKEN_IDENTIFIER:
            {
                struct ShvType type = tokenToType(*tok, ctx);
                tok++;
                char *name = strings + tok->value;
                char *typeKey = atTypeStr(name, 0);
                htSetShvType(ctx->identifiers, typeKey, &type);
                free(typeKey);

                LLVMBuilderRef start = LLVMCreateBuilderInContext(ctx->context);
                LLVMPositionBuilder(start, firstBb, NULL);
                LLVMValueRef val = LLVMBuildAlloca(start, type.llvm, name);
                LLVMDisposeBuilder(start);

                htSetPtr(ctx->identifiers, name, val);
                return statementStartsWithId(tok, strings, ctx, firstBb);
            }
            break;
        // Setting existing variable
        case TOKEN_EQUAL:
            {
                char *name = strings + tok->value;
                char *typeKey = atTypeStr(name, 0);
                struct ShvType type = htGetShvType(ctx->identifiers, typeKey);
                free(typeKey);

                LLVMValueRef pAlloca = htGetPtr(ctx->identifiers, name);

                tok += 2;

                LLVMValueRef result = 0;
                tok = compileExpr(&result, type, tok, strings, ctx);
                if(!tok) return NULL;
                else if(tok->symbol != TOKEN_TERMINATE)
                {
                    shvIssuePtFix(
                        SHVERROR_UNEXPECTED_TOKEN, &tok->fpos,
                        "Missing semi-colon.",
                        -1, "%s = <expression>;", name
                    );
                    return NULL;
                }

                LLVMBuildStore(ctx->builder, result, pAlloca);
                return tok + 1;
            }
            break;
        // Calling fn without args
        case TOKEN_TERMINATE:
            NOT_IMPLEMENTED();
            break;
        // Calling fn with args
        case TOKEN_SHOVE:
            NOT_IMPLEMENTED();
            break;
        default:
            shvIssue(
                SHVERROR_UNEXPECTED_TOKEN,
                &tok->fpos,
                "Unexpected token.",
                "Unexpected token '%s' after identifier.",
                getTokenString((tok + 1)->symbol)
            );
            return NULL;
    }
}

struct Token *irDefineBranch(
    struct FileContext *ctx,
    struct ShvType returnType,
    struct Token *tokens,
    char *strings,
    LLVMBasicBlockRef firstBb
)
{
    struct Token *tok = tokens;
    while(true)
    {
        switch(tok->symbol)
        {
            case TOKEN_IDENTIFIER:
                tok = statementStartsWithId(tok, strings, ctx, firstBb);
                if(!tok) return NULL;
                break;
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
                        tok,
                        strings,
                        ctx
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
    atToken = irDefineBranch(ctx, returnType, atToken, strings, bb);
    
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