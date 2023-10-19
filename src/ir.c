#include <error.h>
#include <llvm-c/Core.h>
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
                       LLVMModuleRef *module
)
{
    *context = LLVMContextCreate();
    *builder = LLVMCreateBuilderInContext(*context);

    *module =
        LLVMModuleCreateWithNameInContext("test.shv", *context);
}

static void destroyLlvm(LLVMContextRef context,
                        LLVMBuilderRef builder,
                        LLVMModuleRef module
)
{
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

static struct ShvType htGetShvType(hashtable_T *ht, const char *key)
{
    return *(struct ShvType*)htGetBuffer(ht, key, strlen(key));
}

static bool htSetShvType(hashtable_T *ht, const char *key, struct ShvType *val)
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
        LLVMValueRef fn;
        LLVMTypeRef typeFn;
        struct ShvType parameterTypes[128];
        uint32_t parameterCount;
        struct ShvType returnType;
        size_t nameLen = strlen(strings + name.value);

        struct Token *atToken = irDeclareFn(
            tokens,
            ctx,
            &fn,
            &typeFn,
            &returnType,
            parameterTypes,
            &parameterCount
        );

        // irDeclareFn erred
        if(!atToken) return NULL;

        struct ShvType fnType =
        {
            .llvm = typeFn,
            .isSigned = 0
        };
        htSetShvType(ctx->identifiers, strings + name.value, &fnType);

        char *metaName = mallocedStrConcat(
            "return@", sizeof("return@") - 1,
            strings + name.value, nameLen
        );
        htSetShvType(ctx->identifiers, metaName, &returnType);

        // Name will be left over from last call
        memcpy(metaName, "params@", sizeof("params@") - 1);
        htSetBuffer(ctx->identifiers, metaName,
            nameLen + sizeof("params@"),
            (char*)parameterTypes,
            parameterCount * sizeof(struct ShvType)
        );

        free(metaName);

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
            fn, atToken, strings, ctx,
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
                          LLVMBuilderRef builder
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
    createLlvm(&context, &builder, &module);

    bool success = compileTokens(tokens, strings, context, module, builder);

    LLVMPrintModuleToFile(module, "./bin/testout.ir", NULL);

    destroyLlvm(context, builder, module);

    return success;
}