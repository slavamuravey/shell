#ifndef TOKENIZER_SENTRY
#define TOKENIZER_SENTRY

#include <stdbool.h>
#include "dynamic_char_array.h"
#include "word_item.h"

struct tokenizer {
    bool quote_mode_enabled;
    bool char_escape_mode_enabled;
    struct dynamic_char_array *tmp_char_array;
    bool within_word;
    struct word_item *first;
    struct word_item *last;
};

struct tokenizer *tokenizer_create();
void tokenizer_clear(struct tokenizer *t);
void tokenizer_accept_char(struct tokenizer *t, char c);
void tokenizer_destroy(struct tokenizer *t);

#endif
