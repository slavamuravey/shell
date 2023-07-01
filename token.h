#ifndef TOKEN_SENTRY
#define TOKEN_SENTRY

#include <stdbool.h>
#include "dynamic_array.h"

struct token {
    char *token;
    bool separator;
};

struct dynamic_array *tokens_array_create();
void tokens_array_destroy(struct dynamic_array *tokens);

#endif
