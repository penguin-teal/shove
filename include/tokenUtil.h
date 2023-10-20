#include <stdio.h>
#include "token.h"

/**
 * Gets a human readable string.
 * This is basically the "*" part of the corresponding TOKEN_* macro.
 * If it represents a keyword it will be all lower-case, or if it
 * is rather a "placeholder," such as `Identifier` it will start with
 * an upper-case letter (with the exception of `EOF`).
 *
 * @remarks The string must NOT be freed or managed by the caller.
 * @return A null-terminated string identifying the given token symbol,
 *         or `"Unknown"` if invalid.
 */
char* getTokenString(int32_t token);