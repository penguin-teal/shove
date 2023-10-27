#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "list.h"

struct StringList
{
    uint64_t *ptrBuffer;
    size_t ptrCapacity;
    size_t ptrLength;

    char *dataBuffer;
    size_t dataCapacity;
    size_t dataLength;
};

static size_t toQWord(size_t sz)
{
    return sz - sz % sizeof(uint64_t) + sizeof(uint64_t);
}

static void *bufferGuarantee(void *buffer, size_t *current, size_t needed)
{
    size_t has = *current;
    if(needed > has)
    {
        if(has * 2 > needed) has *= 2;
        else has = toQWord(needed);

        void *newBuffer = realloc(buffer, has);
        if(!newBuffer)
        {
            ERR_HEAPFAIL("Resize Buffer");
            *current = 0;
            return buffer;
        }

        *current = has;
        return newBuffer;
    }
    else return buffer;
}

string_list_T *createStringList(uint64_t initialCapacity)
{
    uint64_t *ptrBuffer = NULL;
    char *dataBuffer = NULL;

    string_list_T *list = malloc(sizeof(string_list_T));
    if(!list) goto HeapFail;

    list->ptrCapacity = toQWord(initialCapacity * sizeof(uint64_t));
    list->ptrLength = 0;
    ptrBuffer = malloc(list->ptrCapacity);
    if(!ptrBuffer) goto HeapFail;
    list->ptrBuffer = ptrBuffer;


    list->dataCapacity = toQWord(initialCapacity * 16);
    list->dataLength = 0;
    dataBuffer = malloc(list->dataCapacity);
    if(!dataBuffer) goto HeapFail;
    list->dataBuffer = dataBuffer;

    return list;

HeapFail:
    ERR_HEAPFAIL("String List");
    if(list) free(list);
    if(ptrBuffer) free(ptrBuffer);
    if(dataBuffer) free(dataBuffer);
    return NULL;
}

void destroyStringList(string_list_T *list)
{
    free(list->ptrBuffer);
    free(list->dataBuffer);
    free(list);
}

char *stringListAt(string_list_T *list, uint64_t index)
{
    return list->dataBuffer + *(list->ptrBuffer + index) + sizeof(uint64_t);
}

bool stringListPush(string_list_T *list, char *s, size_t n)
{
    if(n == 0) n = strlen(s) + 1;

    uint64_t index = list->ptrLength;
    size_t dataIndex = list->dataLength;

    size_t newPtrLen = list->ptrLength + 1;
    size_t newPtrCap = list->ptrCapacity;
    list->ptrBuffer = bufferGuarantee(list->ptrBuffer, &newPtrCap, newPtrLen);
    if(!list->ptrCapacity) return false;
    else
    {
        list->ptrCapacity = newPtrCap;
    }

    size_t newDataLen = toQWord(list->dataLength + n + sizeof(uint64_t));
    size_t newDataCap = list->dataCapacity;
    list->dataBuffer = bufferGuarantee(list->dataBuffer, &newDataCap, newDataLen);
    if(!list->dataCapacity) return false;
    
    list->dataLength = newDataLen;
    list->dataCapacity = newDataCap;

    list->ptrLength = newPtrLen;

    memcpy(list->dataBuffer + dataIndex, &index, sizeof(uint64_t));
    memcpy(list->dataBuffer + dataIndex + sizeof(uint64_t), s, n);
    list->ptrBuffer[index] = dataIndex;

    return true;

}

uint64_t stringListCount(string_list_T *list)
{
    return list->ptrLength;
}

char *stringListIterate(string_list_T *list, char *s)
{
    uint64_t nextIndex;
    if(s) nextIndex = *(((uint64_t*)s) - 1) + 1;
    else nextIndex = 0;

    if(nextIndex >= list->ptrLength) return NULL;

    return stringListAt(list, nextIndex);
}

/**
 * `malloc` s a string of every string value in the list concatenated together
 * with each item surrounded by single quotes and space separated and escaped.
 */
char *stringListMallocedConcatAllSystem(string_list_T *list)
{
    // near exact size plus some extra quad word padding
    char *s = malloc(list->dataLength - sizeof(uint64_t) * list->ptrLength);
    size_t sLen = 0;

    size_t len = stringListCount(list);
    for(uint64_t i = 0; i < len; i++)
    {
        if(i != 0) s[sLen++] = ' ';
        s[sLen++] = '\'';

        char *item = stringListAt(list, i);
        char c;
        uint64_t j = 0;
        while((c = item[j++]))
        {
            switch(c)
            {
                case '\\':
                case '"':
                case '\'':
                    s[sLen++] = '\\';
                    // FALLTHRU
                default:
                    s[sLen++] = c;
                    break;
            }
        }

        s[sLen++] = '\'';
    }

    s[sLen] = '\0';
    return s;
}