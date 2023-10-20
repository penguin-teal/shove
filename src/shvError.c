#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include "shvError.h"

static uint32_t severities[SHVERROR_MAX + 1];
static uint32_t errorCount = 0;
static uint32_t warningCount = 0;

static void inline vshvReport(uint32_t severity,
                           uint32_t errorId,
                           struct FilePos *f,
                           const char *fix,
                           const char *msgFormat,
                           va_list vaArgs
)
{
    char startCol[2 + 1];
    char ending[7 + 4 + 1];
    uint32_t thisCount;
    if(severity == SEVERITY_WARNING)
    {
        memcpy(startCol, "33", 3);
        memcpy(ending, "Warning\x1B[0m", 12);
        thisCount = ++warningCount;
    }
    // error
    else
    {
        memcpy(startCol, "31", 3);
        memcpy(ending, "Error\x1B[0m", 10);
        thisCount = ++errorCount;
    }

    fprintf(stderr,
        "\x1B[%sm(%u) Compile %s ",
        startCol,
        thisCount,
        ending
    );
    fprintf(stderr,
        "0x%x at %s:%u:%u => ",
        errorId,
        f->fileName,
        f->line,
        f->col
    );
    vfprintf(stderr, msgFormat, vaArgs);
    fprintf(stderr, "\n\tFix: %s\n", fix);
}

bool vshvIssue(uint32_t errorId,
                  struct FilePos *f,
                  const char *fix,
                  const char *msgFormat,
                  va_list vaArgs
)
{
    vshvReport(severities[errorId], errorId, f, fix, msgFormat, vaArgs);
    return severities[errorId] == SEVERITY_ERROR;
}

bool shvIssue(uint32_t errorId, struct FilePos *f, const char *fix, const char *msgFormat, ...)
{
    va_list va;
    va_start(va, msgFormat);
    vshvReport(severities[errorId], errorId, f, fix, msgFormat, va);
    va_end(va);
    return severities[errorId] == SEVERITY_ERROR;
}

bool setShvIssueSeverity(uint32_t errorId, uint32_t severity)
{
    switch(severity)
    {
        case SHVERROR_MAX_PARAMS:
        case SHVERROR_UNEXPECTED_TOKEN:
        case SHVERROR_EXTRA_DECIMAL_POINT:
            return false;
        default:
            severities[errorId] = severity;
            return true;
    }
}

void setDefaultShvIssueSeverities(void)
{
    severities[SHVERROR_MAX_PARAMS] = SEVERITY_ERROR;
    severities[SHVERROR_MISMATCH_SHOVE] = SEVERITY_ERROR;
    severities[SHVERROR_UNEXPECTED_TOKEN] = SEVERITY_ERROR;
    severities[SHVERROR_EXTRA_DECIMAL_POINT] = SEVERITY_ERROR;
    severities[SHVERROR_NO_DEF_NO_EXTERN] = SEVERITY_WARNING;
    severities[SHVERROR_UNREACHABLE_CODE] = SEVERITY_WARNING;
}