#pragma once

#include <stdio.h>
#include <stdbool.h>
#include "token.h"

bool lexFile(FILE *src, size_t srcSize, struct Token **tokens, char **strings);