#include "defines.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

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
char* getTokenString(int32_t token)
{
    // TODO: finish list
    switch(token)
    {
        case TOKEN_EOF:
            return "EOF";
        case TOKEN_NAMESPACE:
            return "namespace";
        case TOKEN_IMPORT:
            return "import";
        case TOKEN_EXTERN:
            return "extern";
        case TOKEN_TABLE:
            return "table";
        case TOKEN_INTEGER:
            return "Integer";
        case TOKEN_FLOAT:
            return "Float";
        case TOKEN_IDENTIFIER:
            return "Identifier";
        case TOKEN_RETURN:
            return "return";
        case TOKEN_BITWISE_NOT:
            return "~";
        case TOKEN_BANG:
            return "!";
        case TOKEN_PLUS:
            return "+";
        case TOKEN_MINUS:
            return "-";
        case TOKEN_STAR:
            return "*";
        case TOKEN_SLASH:
            return "/";
        case TOKEN_MODULO:
            return "%";
        case TOKEN_AMPERSAND:
            return "&";
        case TOKEN_BITWISE_OR:  
            return "|";
        case TOKEN_BITWISE_XOR:
            return "^";
        case TOKEN_LOGICAL_AND:
            return "&&";
        case TOKEN_LOGICAL_OR:
            return "||";
        case TOKEN_GT:
            return ">";
        case TOKEN_LT:
            return "<";
        case TOKEN_GTE:
            return ">=";
        case TOKEN_LTE:
            return "<=";
        case TOKEN_POW:
            return "%%";
        case TOKEN_SHOVE:
            return "Shove";
        case TOKEN_INTO_NAMESPACE:
            return "::";
        case TOKEN_HASH:
            return "hash";
        case TOKEN_CMP:
            return "cmp";
        case TOKEN_EQUAL:
            return "=";
        case TOKEN_NEW:
            return "new";
        case TOKEN_TERMINATE:
            return ";";
        case TOKEN_SHIFT_LEFT:
            return "<<";
        case TOKEN_SHIFT_RIGHT:
            return ">>";
        case TOKEN_BLOCK_OPEN:
            return "{";
        case TOKEN_BLOCK_CLOSE:
            return "}";
        case TOKEN_THIS:
            return "this";
        case TOKEN_VOID:
            return "void";
        case TOKEN_PAREN_OPEN:
            return "(";
        case TOKEN_PAREN_CLOSE:
            return ")";
        default:
            return "Unknown";
    }
}