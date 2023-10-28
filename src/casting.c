#include <llvm-c/Core.h>
#include <stdbool.h>
#include "shvType.h"
#include "shvError.h"
#include "ir.h"

/**
 * Does nothing or promotes bit size if needed, otherwise fails.
 */
bool implicitCast(
    struct FileContext *ctx,
    struct FilePos *fpos,
    struct ShvType *given,
    struct ShvType *target
)
{
    if(given->isSigned != target->isSigned)
    {
        if(shvIssue(
            SHVERROR_ILLEGAL_IMPLICIT_CAST,
            fpos,
            "The types don't match. If this was intentional, cast it.",
            "Illegal implicit cast from %ssigned '%s' to %ssigned '%s'.",
            given->isSigned ? "" : "un",
            getShvTypeName(given),
            target->isSigned ? "" : "un",
            getShvTypeName(target)
        )) return false;
    }

    if(given->bitSize > target->bitSize)
    {
        if(shvIssue(
            SHVERROR_ILLEGAL_IMPLICIT_CAST,
            fpos,
            "The types don't match. If this was intentional, cast it.",
            "Cannot implicitly cast from '%s' of %lu bits to "
            "a smaller-sized type '%s' of %lu bits.",
            getShvTypeName(given),
            given->bitSize,
            getShvTypeName(target),
            target->bitSize
        )) return false;
    }

    return true;

}