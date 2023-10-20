#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ERR(...) fprintf(stderr, __VA_ARGS__)

#define ERR_HEAPFAIL(name) ERR("Failed to allocate heap memory for '" name "'.\n")

#define NOT_IMPLEMENTED(feature) ERR(feature "Not Implemented")