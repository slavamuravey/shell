#include <stdlib.h>
#include <string.h>
#include "dynamic_array.h"

struct dynamic_array *dynamic_array_create(size_t cap, size_t width)
{
    struct dynamic_array *da = malloc(sizeof(struct dynamic_array));
    da->len = 0;
    da->cap = cap;
    da->width = width;
    da->ptr = malloc(da->width * cap);

    return da;
}

void dynamic_array_append(struct dynamic_array *da, void *ptr)
{
    da->len++;

    if (da->len > da->cap) {
        da->cap *= 2;
        da->ptr = realloc(da->ptr, da->width * da->cap);
    }

    memcpy((char *)da->ptr + (da->len - 1) * da->width, ptr, da->width);
}
