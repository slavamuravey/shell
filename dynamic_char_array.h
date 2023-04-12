#ifndef DYNAMIC_CHAR_ARRAY_SENTRY
#define DYNAMIC_CHAR_ARRAY_SENTRY

#include <unistd.h>

struct dynamic_char_array {
    size_t len;
    size_t cap;
    char *ptr;
};

struct dynamic_char_array *dynamic_char_array_create(size_t cap);
void dynamic_char_array_append(struct dynamic_char_array *dca, char c);
void dynamic_char_array_destroy(struct dynamic_char_array *dca);

#endif
