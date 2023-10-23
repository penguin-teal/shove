#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "fileIo.h"
#include "lexer.h"
#include "ir.h"
#include "shvError.h"
#include "appArgs.h"

int main(int argc, char **argv)
{
    int ret = 0;

    struct AppArgs appArgs = { 0 };
    if(!doArgp(&appArgs, argc, argv)) return 2;

    // Right now we're just gonna open this test
    FILE *f = fopen("./tests/add/main.shv", "r");
    // If file couldn't be opened
    if(!f)
    {
        ERR("Failed to open source file.\n");
        // 2 indicates user error --- we got fed an invalid path
        return 2;
    }
    else
    {
        char fileName[] = "main.shv";
        // Get file size
        // lexFile uses this as a basis for deciding the initial capacity
        // of vectors
        size_t size = fGetSize(f);

        struct Token *tokens;
        char *strings;

        setDefaultShvIssueSeverities();

        if(!lexFile(f, size, fileName, &tokens, &strings))
        {
            // We don't have to free tokens or strings on fail
            fprintf(
                stderr,
                "Compilation Failed: Lexing Failed\n"
            );
            ret = 1;
            goto Cleanup;
        }

        if(!compileLlvm(tokens, strings))
        {
            // We have to free tokens and strings when this fails
            // since lexFile succeeded
            fprintf(
                stderr,
                "Compilation Failed: Compiling Failed\n"
            );
            ret = 1;
            free(tokens);
            free(strings);
            goto Cleanup;
        }

        free(tokens);
        free(strings);
    }

Cleanup:
    fclose(f);
    return ret;
}