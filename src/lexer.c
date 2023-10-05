#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"

static int getToken(FILE *src, int *ch, char *id, size_t idMax, uint64_t *numberLiteral, int lastTok)
{
    int lastChar = *ch;

    while(isspace(lastChar)) lastChar = getc(src);

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
        else return TOKEN_IDENTIFIER;
    }
    else if(isdigit(lastChar))
    {
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
            id[idOffset++] = lastChar;
        }
        while(isdigit((lastChar = getc(src))));

        id[idOffset] = '\0';
        *numberLiteral = strtoll(id, &end, 10);
        *ch = lastChar;
        return end == id ? 0 : TOKEN_INTEGER;

    }
    else if(lastChar == EOF)
    {
        *ch = lastChar;
        return TOKEN_EOF;
    }
    else
    {
        switch(lastChar)
        {
            case '-':
                break;
            case '+':
                break;
            case '*':
                break;
            case '/':
                break;
            case '%':
                break;
            case '^':
                break;
            case '&':
                break;
            case '|':
                break;
            case '!':
                break;
            case '~':
                break;
            case '=':
                break;
            case ';':
                break;
            case '<':
                break;
            case '>':
                break;
        }

        return 0;
    }

}

void lexFile(FILE *src)
{
    int tok = -1;
    int ch = '\n';
    char identifier[1024];
    size_t identifierOffset = 0;
    uint64_t numberLiteral = 0x0;

    do
    {
        tok = getToken(src, &ch, identifier, sizeof identifier, &numberLiteral, tok);
    }
    while(tok && tok != TOKEN_EOF);

}