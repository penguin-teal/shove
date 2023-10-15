#include <stdint.h>
#include <stdarg.h>

void vsyntaxErr(uint32_t errorId,
                struct FilePos *f,
                const char *fix,
                const char *msgFormat,
                va_list vaArgs
);

void syntaxErr(uint32_t errorId, struct FilePos *f, const char *fix, const char *msgFormat, ...);
