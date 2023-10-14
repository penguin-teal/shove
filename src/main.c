#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "defines.h"
#include "lexer.h"
#include "ir.h"

int main(int argc, char **argv)
{
    int ret = 0;
    FILE *f = fopen("./tests/add/main.shv", "r");
    if(!f)
    {
        ERR("Failed top open source file.\n");
        return 2;
    }
    else
    {
        struct stat statStruct;
        int descriptor = fileno(f);
        fstat(descriptor, &statStruct);
        size_t size = statStruct.st_size;

        struct Token *tokens;
        char *strings;

        if(!lexFile(f, size, &tokens, &strings))
        {
            ret = 1;
            goto Cleanup;
        }

        if(!compileLlvm(tokens, strings))
        {
            ret = 1;
            goto Cleanup;
        }

        free(tokens);
        free(strings);
    }

Cleanup:
    fclose(f);
    return ret;
}