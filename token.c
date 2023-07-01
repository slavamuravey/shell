#include <stdlib.h>
#include "token.h"

struct dynamic_array *tokens_create()
{
    struct dynamic_array *tokens = dynamic_array_create(4, sizeof(struct token));

    return tokens;
}

void tokens_destroy(struct dynamic_array *tokens)
{
    int i;
    for (i = 0; i < tokens->len; i++) {
        struct token *data = tokens->ptr;
        struct token token = data[i];
        free(token.token);
    }

    free(tokens->ptr);
    free(tokens);
}
