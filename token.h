#ifndef TOKEN_SENTRY
#define TOKEN_SENTRY

#include <stdbool.h>
#include "dynamic_array.h"

enum token_type {
    TOKEN_TYPE_WORD,
    TOKEN_TYPE_ASYNC,
    TOKEN_TYPE_AND,
    TOKEN_TYPE_PIPE,
    TOKEN_TYPE_OR,
    TOKEN_TYPE_REDIRECT_INPUT,
    TOKEN_TYPE_REDIRECT_OUTPUT,
    TOKEN_TYPE_REDIRECT_OUTPUT_APPEND,
    TOKEN_TYPE_SUBSHELL_START,
    TOKEN_TYPE_SUBSHELL_END,
    TOKEN_TYPE_COMMAND_SEPARATOR
};

struct token {
    enum token_type type;
    char *text;
};

struct token *token_create(enum token_type type, char *text);
struct dynamic_array *tokens_create();
void tokens_destroy(struct dynamic_array *tokens);

#endif
