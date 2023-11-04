#ifndef DYNAMIC_ARRAY_H_SENTRY
#define DYNAMIC_ARRAY_H_SENTRY

#include <unistd.h>

struct dynamic_array {
    size_t len;
    size_t cap;
    size_t width;
    void *ptr;
};

struct dynamic_array *dynamic_array_create(size_t cap, size_t width);
void dynamic_array_append(struct dynamic_array *da, void *ptr);

#endif
