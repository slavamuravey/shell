#ifndef TOKENIZER_SENTRY
#define TOKENIZER_SENTRY

#include <stdbool.h>
#include "dynamic_char_array.h"
#include "token.h"

struct tokenizer {
    bool quote_mode_enabled;
    bool char_escape_mode_enabled;
    struct dynamic_char_array *tmp_word;
    struct dynamic_char_array *tmp_separator;
    bool within_word;
    struct token *first;
    struct token *last;
};

struct tokenize_data {
    struct token *token_first;
};

struct tokenize_error {
    char *message;
};

struct tokenizer *tokenizer_create();
void tokenizer_tokenize(struct tokenizer *t, const char *str, struct tokenize_data **data, struct tokenize_error **error);
void tokenizer_destroy(struct tokenizer *t);

#endif
