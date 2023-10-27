#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "defines.h"
#include "strOp.h"

static char *findLinker()
{
    if(!system("which clang > /dev/null 2>&1"))
    {
        return "clang %s -o %s";
    }
    else if(!system("which gcc > /dev/null 2>&1"))
    {
        return "gcc %s -o %s";
    }
    else
    {
        ERR(
            "Couldn't automatically find a linker.\n"
            "  - Tried Clang\n"
            "  - Tried GCC\n"
            "Not linking.\n"
        );
        return NULL;
    }
}

bool linkObjects(
    const char *outFName,
    string_list_T *objFNames,
    const char *linker,
    bool verbose
)
{
    char *command;
    if(linker)
    {
        mallocedReplaceStr(linker, '%', "%s", 2, 1, &command);
        char *percent = strchr(command, '%');
        if(percent && strchr(percent + 1, '%'))
        {
            ERR(
                "Extra '%%' found in the linker string given. "
                "Not linking.\n"
            );
            free(command);
            return false;
        }
    }
    else
    {
        command = findLinker();
        if(!command) return false;
    }

    char *objInput = stringListMallocedConcatAllSystem(objFNames);
    
    size_t linkerSSz = strlen(command) + strlen(objInput) + strlen(outFName);
    char *linkerS = malloc(linkerSSz);
    int snpf = snprintf(linkerS, linkerSSz, command, objInput, outFName);

    bool success;
    if(snpf >= 0 && (size_t)snpf < linkerSSz)
    {
        if(verbose)
        {
            printf("Linking with: %s\n", linkerS);
        }
        success = !system(linkerS);
    }
    else
    {
        ERR(
            "Internal Error: "
            "Malloced linker program string not big enough for format. "
            "Linker format string:\n\t%s\n",
            command
        );
        success = false;
    }

    free(linkerS);
    free(objInput);
    if(linker) free(command);

    return success;
}