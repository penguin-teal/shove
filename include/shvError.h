#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

struct FilePos
{
    const char *fileName;
    uint32_t line;
    uint32_t col;
    uint32_t len;
};

enum ShvError
{
    SHVERROR_MAX_PARAMS = 0x0001,
    SHVERROR_MISMATCH_SHOVE = 0x0002,
    SHVERROR_UNEXPECTED_TOKEN = 0x0003,
    SHVERROR_NO_DEF_NO_EXTERN = 0x0004,
    SHVERROR_UNREACHABLE_CODE = 0x0005,
    SHVERROR_EXTRA_DECIMAL_POINT = 0x0006,
    SHVERROR_ILLEGAL_IMPLICIT_CAST = 0x0007,

    SHVERROR_MAX = SHVERROR_ILLEGAL_IMPLICIT_CAST
};

enum ShvSeverity
{
    SEVERITY_ERROR = 1,
    SEVERITY_WARNING = 2
};

bool vshvIssue(uint32_t errorId,
                  struct FilePos *f,
                  const char *fix,
                  const char *msgFormat,
                  va_list vaArgs
);

bool shvIssue(uint32_t errorId, struct FilePos *f, const char *fix, const char *msgFormat, ...);

bool shvIssuePtFix(
    uint32_t errorId,
    struct FilePos *f,
    const char *msg,
    int32_t ptToIndex,
    const char *fixFmt,
    ...
);

bool setShvIssueSeverity(uint32_t errorId, uint32_t severity);

void setDefaultShvIssueSeverities(void);

void setStreetRulesShvIssueSeverities(void);

void setPedanticShvIssueSeverities(void);

bool setShvIssueString(char *s, uint32_t severity);