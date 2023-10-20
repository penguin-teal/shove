#include <stdlib.h>
#include <hashedbrown.h>
#include "strOp.h"
#include "shvType.h"
#include "ir.h"

struct ShvType htblGetType(hashtable_T *ht, char *name)
{
    char *typeKey = atTypeStr(name, 0);
    struct ShvType t = htGetShvType(ht, typeKey);
    free(typeKey);
    return t;
}