#include <string.h>
#include <llvm-c/Core.h>
#include "shvType.h"

char *getShvTypeName(struct ShvType *t)
{
    if(t->friendly) return t->friendly;

    LLVMTypeKind kind = LLVMGetTypeKind(t->llvm);
    if(kind == LLVMFunctionTypeKind) return "Unknown Function";
    else return "Unknown Integer";
}

bool shvTypesAreCopies(struct ShvType *a, struct ShvType *b)
{
    return !memcmp(a, b, sizeof(struct ShvType));
}