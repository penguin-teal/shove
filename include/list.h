#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct StringList string_list_T;

string_list_T *createStringList(uint64_t initialCapacity);

void destroyStringList(string_list_T *list);

char *stringListAt(string_list_T *list, uint64_t index);

bool stringListPush(string_list_T *list, char *s, size_t n);

uint64_t stringListCount(string_list_T *list);

char *stringListIterate(string_list_T *list, char *s);

char *stringListMallocedConcatAllSystem(string_list_T *list);