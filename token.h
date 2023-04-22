#ifndef TOKEN_SENTRY
#define TOKEN_SENTRY

#include <stdbool.h>

struct token {
    char *token;
    bool separator;
    struct token *next;   
};

struct token *token_create(char *token, bool separator);
void token_push(struct token **first, struct token **last, struct token *new_item);
void token_destroy(struct token *first);

#endif
