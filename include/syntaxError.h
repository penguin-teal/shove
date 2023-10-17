#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

bool vsyntaxIssue(uint32_t errorId,
                  struct FilePos *f,
                  const char *fix,
                  const char *msgFormat,
                  va_list vaArgs
);

bool syntaxIssue(uint32_t errorId, struct FilePos *f, const char *fix, const char *msgFormat, ...);

bool setSyntaxIssueSeverity(uint32_t errorId, uint32_t severity);

void setDefaultSyntaxIssueSeverities(void);