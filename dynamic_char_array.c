#include <stdlib.h>
#include "dynamic_char_array.h"

struct dynamic_char_array *dynamic_char_array_create(size_t cap)
{
    struct dynamic_char_array *dca = malloc(sizeof(struct dynamic_char_array));
    dca->len = 0;
    dca->cap = cap;
    dca->ptr = malloc(sizeof(char) * cap);

    return dca;
}

void dynamic_char_array_append(struct dynamic_char_array *dca, char c)
{
    dca->len++;

    if (dca->len > dca->cap) {
        dca->cap *= 2;
        dca->ptr = realloc(dca->ptr, dca->cap);
    }

    dca->ptr[dca->len - 1] = c;
}

void dynamic_char_array_destroy(struct dynamic_char_array *dca)
{
    free(dca->ptr);
    free(dca);
}
