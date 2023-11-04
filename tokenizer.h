#ifndef TOKENIZER_H_SENTRY
#define TOKENIZER_H_SENTRY

#include <stdbool.h>
#include "dynamic_array.h"

struct tokenizer {
    bool quote_mode_enabled;
    bool char_escape_mode_enabled;
    struct dynamic_array *tmp_word;
    bool within_word;
    struct dynamic_array *tokens;
};

struct tokenize_data {
    struct dynamic_array *tokens;
};

struct tokenize_error {
    char *message;
};

struct tokenizer *tokenizer_create();
void tokenizer_tokenize(struct tokenizer *t, const char *str, struct tokenize_data **data, struct tokenize_error **error);
void tokenizer_destroy(struct tokenizer *t);

#endif
