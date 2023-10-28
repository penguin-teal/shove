#include <llvm-c/Core.h>
#include "ir.h"

void populatePrimitiveTypes(struct FileContext *ctx)
{
    struct ShvType type;

    type.llvm = LLVMInt32TypeInContext(ctx->context);
    type.isSigned = true;
    type.bitSize = 32;
    type.friendly = "i32";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    // leftover llvm
    type.isSigned = false;
    // leftover bitSize
    type.friendly = "u32";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    type.llvm = LLVMInt64TypeInContext(ctx->context);
    type.isSigned = true;
    type.bitSize = 64;
    type.friendly = "i64";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    // leftover llvm
    type.isSigned = false;
    // leftover bitSize
    type.friendly = "u64";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    type.llvm = LLVMInt16TypeInContext(ctx->context);
    type.isSigned = true;
    type.bitSize = 16;
    type.friendly = "i16";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    // leftover llvm
    type.isSigned = false;
    // leftover bitSize
    type.friendly = "u16";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    type.llvm = LLVMInt8TypeInContext(ctx->context);
    type.isSigned = true;
    type.bitSize = 8;
    type.friendly = "i8";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    // leftover llvm
    type.isSigned = false;
    // leftover bitSize
    type.friendly = "u8";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    type.llvm = LLVMFloatTypeInContext(ctx->context);
    type.isSigned = true;
    type.bitSize = 32;
    type.friendly = "f32";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );

    type.llvm = LLVMDoubleTypeInContext(ctx->context);
    type.isSigned = true;
    type.bitSize = 64;
    type.friendly = "f64";
    htSetShvType(
        ctx->identifiers,
        type.friendly,
        &type
    );
}