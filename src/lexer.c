#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"
#include "shvError.h"
#include "token.h"
#include "ir.h"

static void fAdvance(struct FilePos *fpos, int ch)
{
    fpos->col++;

    if (ch == '\n')
    {
        fpos->line++;
        fpos->col = 0;
    }
}

/**
 * Gets a token from the Shove source file.
 * @param src The in source file.
 * @param ch The last character read. Just pass in LF if this is the first read. This will be set to the last character read by this function.
 * @param id The place to put a string. For identifiers, will contain the identifier string. Otherwise may be used as a workspace.
            Any data inputted may be overwritten.
 * @param idMax Buffer size of id.
 * @param numberLiteral Spot for putting numeric values. Will be filled in if is a number literla token.
 * @param fpos A pointer to a FilePos struct to fill in. This will be read and overwritten.
               First read should pass in `{ .fileName = <name>, .line = 1, .col = 0 }`.
 * @param lastTok The last token read. Just pass in TOKEN_EOF if not read yet.
 * @return The token.
**/
static int32_t getToken(FILE *src, int *ch, char *id,
                        size_t idMax, uint64_t *numberLiteral,
                        struct FilePos *fpos, int lastTok
)
{
    int lastChar = *ch;
    fAdvance(fpos, lastChar);

    while(isspace(lastChar))
    {
        lastChar = getc(src);
        fAdvance(fpos, lastChar);
    }


    if(isalpha(lastChar))
    {
        size_t idOffset = 0;
        id[idOffset++] = lastChar;
        while (isalnum((lastChar = getc(src))))
        {
            id[idOffset++] = lastChar;
            if(idOffset + 1 >= idMax) break;
        }
        id[idOffset] = '\0';
        *ch = lastChar;

        if(!strncmp(id, "namespace", idMax)) return TOKEN_NAMESPACE;
        else if(!strncmp(id, "import", idMax)) return TOKEN_IMPORT;
        else if(!strncmp(id, "extern", idMax)) return TOKEN_EXTERN;
        else if(!strncmp(id, "table", idMax)) return TOKEN_TABLE;
        else if(!strncmp(id, "cmp", idMax)) return TOKEN_CMP;
        else if(!strncmp(id, "hash", idMax)) return TOKEN_HASH;
        else if(!strncmp(id, "new", idMax)) return TOKEN_NEW;
        else if(!strncmp(id, "return", idMax)) return TOKEN_RETURN;
        else if(!strncmp(id, "this", idMax)) return TOKEN_THIS;
        else return TOKEN_IDENTIFIER;
    }
    else if(isdigit(lastChar))
    {
        bool isFloatingPoint = false;
        size_t idOffset = 0;
        char *end;
        if(lastChar == '0')
        {
            lastChar = getc(src);
            switch (lastChar)
            {
            case 'X':
            case 'x':
                while(isdigit((lastChar = getc(src))) ||
                     (lastChar >= 'a' && lastChar <= 'f') ||
                     (lastChar >= 'A' && lastChar <= 'F'))
                {
                    id[idOffset++] = lastChar;
                }
                id[idOffset] = '\0';
                *numberLiteral = strtoll(id, &end, 16);
                *ch = lastChar;
                return end == id ? 0 : TOKEN_INTEGER;
                // Use 0o instead of just 0 for octals
            case 'O':
            case 'o':
                while((lastChar = getc(src)) >= '0' && lastChar <= '8')
                {
                    id[idOffset++] = lastChar;
                }
                id[idOffset] = '\0';
                *numberLiteral = strtoll(id, &end, 8);
                *ch = lastChar;
                return end == id ? 0 : TOKEN_INTEGER;
            case 'B':
            case 'b':
                while ((lastChar = getc(src)) == '0' || lastChar == '1')
                {
                    id[idOffset++] = lastChar;
                }
                id[idOffset] = '\0';
                *numberLiteral = strtoll(id, &end, 2);
                *ch = lastChar;
                return end == id ? 0 : TOKEN_INTEGER;
            default:
                break;
            }
        }

        do
        {
            if(lastChar == '.')
            {
                if(!isFloatingPoint) isFloatingPoint = true;
                else
                {
                    shvIssue(
                        SHVERROR_EXTRA_DECIMAL_POINT,
                        fpos,
                        "Remove extra '.'.",
                        "Extra decimal point in floating point literal."
                    );
                    return 0;
                }
            }
            id[idOffset++] = lastChar;
        }
        while(isdigit((lastChar = getc(src))) || lastChar == '.');

        id[idOffset] = '\0';
        *ch = lastChar;
        
        if(isFloatingPoint)
        {
            double floating = strtod(id, &end);
            // Assumes double will be <= 64 bits
            memcpy(numberLiteral, &floating, sizeof(uint64_t));

            return end == id ? 0 : TOKEN_FLOAT;
        }
        else
        {
            *numberLiteral = strtoll(id, &end, 10);
            return end == id ? 0 : TOKEN_INTEGER;
        }

    }
    else if(lastChar == EOF)
    {
        *ch = lastChar;
        return TOKEN_EOF;
    }
    else
    {
        *ch = getc(src);
        switch(lastChar)
        {
            case '-':
            if(*ch == '>')
            {
                uint64_t i = 0;
                do
                {
                    *ch = getc(src);
                    *numberLiteral = ++i;
                }
                while(*ch == '>');
                // Return a shove with shove depth in numberLiteral
                return TOKEN_SHOVE;
            }
            else return TOKEN_MINUS;
            case '+':
                return TOKEN_PLUS;
            case '*':
                return TOKEN_STAR;
            case '/':
                return TOKEN_SLASH;
            case '%':
                if(*ch == '%')
                {
                    *ch = getc(src);
                    return TOKEN_POW;
                }
                else return TOKEN_MODULO;
            case '^':
                return TOKEN_BITWISE_XOR;
            case '&':
                if (*ch == '&')
                {
                    *ch = getc(src);
                    return TOKEN_LOGICAL_AND;
                }
                else return TOKEN_AMPERSAND;
            case '|':
                if (*ch == '|')
                {
                    *ch = getc(src);
                    return TOKEN_LOGICAL_OR;
                }
                else return TOKEN_BITWISE_OR;
            case '!':
                return TOKEN_BANG;
            case '~':
                return TOKEN_BITWISE_NOT;
            case '=':
                return TOKEN_EQUAL;
            case ';':
                return TOKEN_TERMINATE;
            case '<':
                if(*ch == '<')
                {
                    *ch = getc(src);
                    return TOKEN_SHIFT_LEFT;
                }
                else if(*ch == '=')
                {
                    *ch = getc(src);
                    return TOKEN_LTE;
                }
                else return TOKEN_LT;
            case '>':
                if(*ch == '>')
                {
                    *ch = getc(src);
                    return TOKEN_SHIFT_RIGHT;
                }
                else if(*ch == '=')
                {
                    *ch = getc(src);
                    return TOKEN_GTE;
                }
                else return TOKEN_GT;
            case ':':
                if(*ch == ':')
                {
                    *ch = getc(src);
                    return TOKEN_INTO_NAMESPACE;
                }
                else return 0;
            case '{':
                return TOKEN_BLOCK_OPEN;
            case '}':
                return TOKEN_BLOCK_CLOSE;
            case '(':
                return TOKEN_PAREN_OPEN;
            case ')':
                return TOKEN_PAREN_CLOSE;
        }

        return 0;
    }

}

static size_t pushSpace(void **space, void **at, size_t size, const void *item, size_t itemSize)
{
    size_t needed = (char*)*at - (char*)*space + itemSize;
    if(needed > size)
    {
        if(size * 2 < needed) size *= 2;
        else size = needed;

        void *newSpace = realloc(*space, size);
        if(!newSpace) return 0;

        *space = newSpace;
    }

    memcpy(*at, item, itemSize);
    *at += itemSize;
    return size;
}

bool lexFile(FILE *src, size_t srcSize, const char *srcName, struct Token **tokens, char **strings)
{
    int32_t tok = -1;
    int ch = '\n';
    char identifier[1024];
    size_t identifierOffset = 0;
    uint64_t numberLiteral = 0x0;
    struct FilePos fpos =
    {
        // This does mean that the struct relies on the caller's string
        .fileName = srcName,

        .line = 1,
        .col = 0
    };
    bool success = false;

    size_t stringSpaceSize = srcSize;
    char *stringSpace = malloc(stringSpaceSize);
    if(!stringSpace)
    {
        ERR_HEAPFAIL("Lexer Identifiers");
        return false;
    }
    char *atStringSpace = stringSpace;

    size_t tokenSpaceSize = srcSize * sizeof(struct Token);
    struct Token *tokenSpace = malloc(tokenSpaceSize);
    if(!tokenSpace)
    {
        ERR_HEAPFAIL("Lexer Tokens");
        return false;
    }
    struct Token *atTokenSpace = tokenSpace;

    struct Token tokStruct;
    size_t offset;

    while(true)
    {
        tok = getToken(
            src,
            &ch,
            identifier,
            sizeof identifier,
            &numberLiteral,
            &fpos,
            tok
        );

        tokStruct.symbol = tok;
        // This will take a copy
        tokStruct.fpos = fpos;

        switch(tok)
        {
            case 0:
                goto Cleanup;
            case TOKEN_EOF:
                goto ExitWhile;
            case TOKEN_INTEGER:
            case TOKEN_FLOAT:
            case TOKEN_SHOVE:
                tokStruct.value = numberLiteral;
                break;
            case TOKEN_IDENTIFIER:
                // Use offset instead of pointers in case we realloc
                offset = atStringSpace - stringSpace;

                size_t strLen = strlen(identifier) + 1;
                stringSpaceSize = pushSpace(
                    (void**)&stringSpace,
                    (void**)&atStringSpace,
                    stringSpaceSize,
                    identifier,
                    strLen
                );
                if(!stringSpaceSize)
                {
                    ERR_HEAPFAIL("Resize Lexer Identifiers");
                    goto Cleanup;
                }

                tokStruct.value = offset;
                break;
            default:
                tokStruct.value = 0x0;
                break;
        }

        tokenSpaceSize = pushSpace(
            (void**)&tokenSpace,
            (void**)&atTokenSpace,
            tokenSpaceSize,
            &tokStruct,
            sizeof(struct Token)
        );
        if(!tokenSpaceSize)
        {
            ERR_HEAPFAIL("Resize Lexer Tokens");
            goto Cleanup;
        }

    }

ExitWhile:
    tokStruct.symbol = TOKEN_EOF;
    tokStruct.value = 0;

    tokenSpaceSize = pushSpace(
        (void**)&tokenSpace,
        (void**)&atTokenSpace,
        tokenSpaceSize,
        &tokStruct,
        sizeof(struct Token)
    );

    // We won't cleanup here since we're returning the buffers
    *tokens = tokenSpace;
    *strings = stringSpace;
    return true;

Cleanup:
    free(stringSpace);
    free(tokenSpace);
    return success;
}