#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * `malloc` s and concatenates two strings and returns the new string.
 * The string must be freed by the caller.
 * @param start The first string.
 * @param startSize The length of the first string (excluding the `NUL`)
                    if known, otherwise `0`.
 * @param end The second string.
* @param endSize The length of the second string (excluding the `NUL`)
                 if known, otherwise `0`.
 */
char *mallocedStrConcat(const char *start, uint64_t startSize,
                        const char *end, uint64_t endSize
)
{
    if(!startSize) startSize = strlen(start);
    if(!endSize) endSize = strlen(end);
    char *str = malloc(startSize + endSize + 1);

    memcpy(str, start, startSize);
    memcpy(str + startSize, end, endSize);
    str[startSize + endSize] = 0;

    return str;
}

char *atTypeStr(const char *name, uint64_t size)
{
    return mallocedStrConcat("type@", sizeof("type@") - 1, name, size);
}

char *atReturnStr(const char *name, uint64_t size)
{
    return mallocedStrConcat("return@", sizeof("return@"), name, size);
}

char *atParamsStr(const char *name, uint64_t size)
{
    return mallocedStrConcat("params@", sizeof("params") - 1, name, size);
}

bool strEndsWith(char *overall, char *substr)
{
    size_t sublen = strlen(substr);
    size_t len = strlen(overall);
    if(sublen > len) return false;
    return !memcmp(overall + len - sublen, substr, sublen);
}