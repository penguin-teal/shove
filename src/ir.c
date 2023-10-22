#include <error.h>
#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <string.h>
#include <hashedbrown.h>
#include <stdlib.h>
#include "ir.h"
#include "irFn.h"
#include "strOp.h"
#include "token.h"
#include "defines.h"
#include "tokenUtil.h"
#include "shvError.h"
#include "shvType.h"

static void createLlvm(LLVMContextRef *context,
                       LLVMBuilderRef *builder,
                       LLVMModuleRef *module,
                       LLVMPassManagerRef *pass
)
{
    *context = LLVMContextCreate();
    *builder = LLVMCreateBuilderInContext(*context);

    *module =
        LLVMModuleCreateWithNameInContext("test.shv", *context);
    
    *pass = LLVMCreateFunctionPassManagerForModule(*module);
}

static void destroyLlvm(LLVMContextRef context,
                        LLVMBuilderRef builder,
                        LLVMModuleRef module,
                        LLVMPassManagerRef pass
)
{
    LLVMDisposePassManager(pass);
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    LLVMContextDispose(context);
}

bool expectToken(int32_t actual, int32_t expected, struct FilePos *fpos, const char *fix)
{
    if(actual == expected) return true;
    else
    {
        shvIssue(
            SHVERROR_UNEXPECTED_TOKEN,
            fpos,
            fix,
            "Expected token '%s' but got '%s'.",
            getTokenString(expected),
            getTokenString(actual)
        );
        return false;
    }
}

struct ShvType htGetShvType(hashtable_T *ht, const char *key)
{
    return *(struct ShvType*)htGetBuffer(ht, key, strlen(key));
}

bool htSetShvType(hashtable_T *ht, const char *key, struct ShvType *val)
{
    return htSetBuffer(
        ht,
        key,
        strlen(key),
        (void*)val,
        sizeof(struct ShvType)
    );
}

struct ShvType tokenToType(struct Token token, struct FileContext *ctx)
{
    return htGetShvType(ctx->identifiers, ctx->strings + token.value);
}

static void populatePrimitiveTypes(struct FileContext *ctx)
{
    struct ShvType type;

    type.llvm = LLVMInt32TypeInContext(ctx->context);
    type.isSigned = true;
    htSetShvType(
        ctx->identifiers,
        "i32",
        &type
    );
}

static struct Token *startsWithIdentifier(struct Token *tokens,
                                          char *strings,
                                          struct FileContext *ctx,
                                          hashtable_T *fnScope
)
{
    struct Token type = tokens[0];
    if(!expectToken(type.symbol, TOKEN_IDENTIFIER, &type.fpos, "Internal error."))
        return NULL;

    struct Token name = tokens[1];
    if(!expectToken(name.symbol, TOKEN_IDENTIFIER, &type.fpos, "Type not followed by name."))
        return NULL;

    struct Token after = tokens[2];
    if(after.symbol == TOKEN_BLOCK_OPEN || after.symbol == TOKEN_SHOVE)
    {
        LLVMValueRef fnVal;
        LLVMTypeRef typeFn;
        struct ShvType parameterTypes[128];
        uint32_t parameterCount;
        struct ShvType returnType;
        char *strName = strings + name.value;
        size_t nameLen = strlen(strName);

        struct Token *atToken = irDeclareFn(
            tokens,
            ctx,
            &fnVal,
            &typeFn,
            &returnType,
            parameterTypes,
            &parameterCount
        );

        // irDeclareFn erred
        if(!atToken) return NULL;

        struct ShvType fnShvType =
        {
            .llvm = typeFn,
            .isSigned = 0
        };
        htSetPtr(ctx->identifiers, strName, fnVal);

        char *returnKey = atReturnStr(strName, nameLen);
        htSetShvType(ctx->identifiers, returnKey, &returnType);
        free(returnKey);

        char *paramsKey = atParamsStr(strName, nameLen);
        htSetBuffer(ctx->identifiers, paramsKey,
            nameLen + sizeof("params@"),
            (char*)parameterTypes,
            parameterCount * sizeof(struct ShvType)
        );
        free(paramsKey);

        char *typeKey = atTypeStr(strName, nameLen);
        htSetShvType(ctx->identifiers, typeKey, &fnShvType);
        free(typeKey);

        // Declare function without defining (i.e. for extern)
        if(atToken->symbol == TOKEN_TERMINATE)
        {
            if(shvIssue(SHVERROR_NO_DEF_NO_EXTERN, &type.fpos,
                "Define function or add \"extern\".",
                "Missing function body without \"extern\"."))
            {
                return NULL;
            }
            else return ++atToken;
        }
        // Compile body
        atToken = irDefineFn(
            fnVal, atToken, strings, ctx,
            fnScope, returnType, parameterTypes
        );

        return atToken;
    }
    else
    {
        shvIssue(
            SHVERROR_UNEXPECTED_TOKEN,
            &after.fpos,
            "A function must have a shove for parameters or a body.",
            "Expected either token '%s' or '%s' but got '%s'.",
            TOKEN_BLOCK_OPEN, TOKEN_SHOVE,
            after.symbol
        );
        return NULL;
    }
}

static bool compileTokens(struct Token *tokens,
                          char *strings,
                          LLVMContextRef context,
                          LLVMModuleRef module,
                          LLVMBuilderRef builder,
                          LLVMPassManagerRef pass
)
{
    bool ret = false;
    struct Token *atToken = tokens;
    char *namespace;

    hashtable_T *fileHashTable = htTableCreate(5000);
    if(!fileHashTable) return NULL;

    hashtable_T *fnScope = htTableCreate(5000);
    if(!fnScope)
    {
        htTableDestroy(fileHashTable);
        return NULL;
    }

    struct FileContext ctx =
    {
        .strings = strings,
        .context = context,
        .module = module,
        .builder = builder,
        .pass = pass,
        .identifiers = fileHashTable
    };
    populatePrimitiveTypes(&ctx);

    while(true)
    {
        struct Token tok = *atToken;
        switch(tok.symbol)
        {
            case TOKEN_EOF:
                goto ExitWhile;
            case TOKEN_NAMESPACE:
                atToken++;
                if(!expectToken(atToken->symbol, TOKEN_IDENTIFIER, &atToken->fpos, "namespace <mynamespace>"))
                    goto Cleanup;
                namespace = strings + (size_t)atToken->value;
                atToken++;
                if(!expectToken(atToken->symbol, TOKEN_TERMINATE, &atToken->fpos, "Add a semicolon."))
                    goto Cleanup;
                atToken++;
                break;
            case TOKEN_IMPORT:
                NOT_IMPLEMENTED();
                goto Cleanup;
            case TOKEN_TABLE:
                NOT_IMPLEMENTED();
                goto Cleanup;
            case TOKEN_EXTERN:
                atToken++;
            case TOKEN_IDENTIFIER:
                atToken = startsWithIdentifier(
                        atToken,
                        strings,
                        &ctx,
                        fnScope
                );
                if(!atToken) goto Cleanup;
                break;
            default:
                shvIssue(
                    SHVERROR_UNEXPECTED_TOKEN,
                    &tok.fpos,
                    "Unexpected token.",
                    "Unexpected token '%s' at top-level.",
                    getTokenString(tok.symbol)
                );
                break;
        }
    }

ExitWhile:
    ret = true;
Cleanup:
    htTableDestroy(fileHashTable);
    htTableDestroy(fnScope);
    return ret;
}

bool compileLlvm(struct Token *tokens, char *strings)
{
    LLVMContextRef context;
    LLVMBuilderRef builder;
    LLVMModuleRef module;
    LLVMPassManagerRef pass;
    createLlvm(&context, &builder, &module, &pass);

    bool success = compileTokens(tokens, strings, context, module, builder, pass);

    LLVMPrintModuleToFile(module, "./bin/testout.ir", NULL);

    destroyLlvm(context, builder, module, pass);

    return success;
}