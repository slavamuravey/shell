#include <stdlib.h>
#include "token.h"

struct token *token_create(char *token, bool separator)
{
    struct token *t = malloc(sizeof(struct token));
    t->token = token;
    t->separator = separator;
    t->next = NULL;

    return t;
}

void token_push(struct token **first, struct token **last, struct token *token)
{
    if (*last) {
        (*last)->next = token;
        *last = (*last)->next;
    } else {
        *first = *last = token;
    }
}

void token_destroy(struct token *first)
{
    struct token *tmp;
    while (first) {
        tmp = first;
        first = first->next;
        free(tmp->token);
        free(tmp);
    }
}
