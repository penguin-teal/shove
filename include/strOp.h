#pragma once

#include <stdint.h>

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
);