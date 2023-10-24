#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "defines.h"

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

uint64_t mallocedReplaceStr(const char *s, char c, const char *substring, uint64_t substrLen, uint32_t limit, char **out)
{
    size_t sz = 256;
    uint64_t destLen = 0;
    *out = malloc(sz);
    if(!*out)
    {
        ERR("Failed to allocate %lu B for string.\n", sz);
        return 0;
    }

    uint32_t found = 0;

    uint64_t i = 0;
    while(true)
    {
        size_t needed = destLen + substrLen;

        if(limit > 0 && found >= limit) needed = destLen + strlen(s + i) + 1;
        else if(s[i] == c) needed = destLen + substrLen;
        else needed = destLen + 1;

        if(needed > sz)
        {
            sz = sz * 2 > needed ? sz * 2 : needed;
            char *newDest = realloc(*out, sz);
            if(newDest) *out = newDest;
            else
            {
                ERR("Failed to reallocate %lu B for string.\n", sz);
                free(*out);
                return 0;
            }
        }
            
        if(limit > 0 && found >= limit)
        {
            size_t len = needed - destLen;
            memcpy(*out + destLen, s + i, len);
            destLen += len;
            break;
        }
        else if(s[i] == c)
        {
            memcpy(*out + destLen, substring, substrLen);
            destLen += substrLen;
            found++;
        }
        else
        {
            (*out)[destLen++] = s[i];
            if(!s[i]) break;
        }

        i++;
    }

    return destLen;
}