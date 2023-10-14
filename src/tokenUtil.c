#include "defines.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

char* getTokenString(int32_t token)
{
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

        case TOKEN_BLOCK_OPEN:
            return "{";
        case TOKEN_BLOCK_CLOSE:
            return "}";
        default:
            return "Unknown";
    }
}