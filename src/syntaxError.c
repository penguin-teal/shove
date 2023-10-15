#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include "shvError.h"

void vsyntaxErr(uint32_t errorId,
                struct FilePos *f,
                const char *fix,
                const char *msgFormat,
                va_list vaArgs
)
{
    fprintf(stderr,
        "Syntax Error 0x%x at %s:%u:%u => ",
        errorId,
        f->fileName,
        f->line,
        f->col
    );
    vfprintf(stderr, msgFormat, vaArgs);
    fprintf(stderr, "\n\tFix: %s\n", fix);
}

void syntaxErr(uint32_t errorId, struct FilePos *f, const char *fix, const char *msgFormat, ...)
{
    va_list va;
    va_start(va, msgFormat);
    vsyntaxErr(errorId, f, fix, msgFormat, va);
    va_end(va);
}
