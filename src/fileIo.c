#include <stdio.h>
#include <sys/stat.h>

/**
 * Gets the size of a file in bytes.
 * @param f The `FILE*` to retrieve the size of.
 * @return The size of the given file, in bytes.
 */
size_t fGetSize(FILE *f)
{
    // Get the size from stat rather than just seeking
    // because the seeking method is non-standard
    struct stat statStruct;
    int descriptor = fileno(f);
    fstat(descriptor, &statStruct);
    return statStruct.st_size;
}