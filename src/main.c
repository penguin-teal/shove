#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "defines.h"
#include "fileIo.h"
#include "lexer.h"
#include "ir.h"
#include "shvError.h"
#include "appArgs.h"
#include "strOp.h"

int main(int argc, char **argv)
{
    struct AppArgs appArgs = { 0 };
    if(!doArgp(&appArgs, argc, argv)) return 2;

    if(appArgs.pedantic) setPedanticShvIssueSeverities();
    else if(appArgs.streetRules) setStreetRulesShvIssueSeverities();
    else setDefaultShvIssueSeverities();

    if(appArgs.warn) setShvIssueString(appArgs.warn, SEVERITY_WARNING);
    if(appArgs.err) setShvIssueString(appArgs.err, SEVERITY_ERROR);

    char *dirName = appArgs.args[0];
    size_t dirNameLen = strlen(dirName);
    DIR *srcDir = opendir(dirName);
    if(!srcDir)
    {
        ERR("Failed to open source directory '%s'.\n", dirName);
        return 2;
    }

    struct dirent *dirEntry;

    size_t fullPathSize = 1024;
    char *fullPath = malloc(fullPathSize);
    if(!fullPath)
    {
        ERR("%lu B malloc failed.\n", fullPathSize);
        closedir(srcDir);
        return 1;
    }

    while((dirEntry = readdir(srcDir)))
    {
        char *fName = dirEntry->d_name;
        size_t fNameLen = strlen(fName);

        if(*fName == '.' && ((fName[1] == '.' && fName[2] == 0) || fName[1] == 0)) continue;

        size_t fullPathLen = dirNameLen + fNameLen + 2;
        if(fullPathSize < fullPathLen)
        {
            fullPathSize = fullPathLen;
            fullPath = realloc(fullPath, fullPathSize);
            if(!fullPath)
            {
                ERR("%lu B realloc failed.", fullPathSize);
                free(fullPath);
                closedir(srcDir);
                return 1;
            }
        }

        memcpy(fullPath, dirName, dirNameLen);
        if(dirName[dirNameLen - 1] != '/' && *fName != '/')
        {
            fullPath[dirNameLen] = '/';
            memcpy(fullPath + dirNameLen + 1, fName, fNameLen + 1);
        }
        else memcpy(fullPath + dirNameLen, fName, fNameLen + 1);

        if(!strEndsWith(fName, ".shv"))
        {
            if(appArgs.verbose)
            {
                printf("Skipping file '%s' because it doesn't end with '.shv'.\n", fullPath);
            }
            continue;
        }

        FILE *f = fopen(fullPath, "r");
        if(!f)
        {
            ERR("Failed to open source file '%s'.\n", fullPath);
            free(fullPath);
            closedir(srcDir);
            return 1;
        }

        if(appArgs.verbose)
        {
            printf("Compiling shove source file '%s'.\n", fullPath);
        }

        size_t fSize = fGetSize(f);

        struct Token *tokens;
        char *strings;

        if(!lexFile(f, fSize, fName, &tokens, &strings))
        {
            // We don't have to free tokens or strings on fail
            ERR(
                "Compilation Failed: Lexing Failed\n"
            );
            free(fullPath);
            fclose(f);
            closedir(srcDir);
            return 1;
        }

        if(!compileLlvm(tokens, strings))
        {
            // We have to free tokens and strings when this fails
            // since lexFile succeeded
            ERR(
                "Compilation Failed: Compiling Failed\n"
            );
            free(fullPath);
            free(tokens);
            free(strings);
            fclose(f);
            closedir(srcDir);
            return 1;
        }

        free(tokens);
        free(strings);

        fclose(f);
    }

    free(fullPath);
    closedir(srcDir);
    return 0;
}