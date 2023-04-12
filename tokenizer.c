#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tokenizer.h"

struct tokenizer *tokenizer_create()
{
    struct tokenizer *t = malloc(sizeof(struct tokenizer));
    t->quote_mode_enabled = false;
    t->char_escape_mode_enabled = false;
    t->tmp_char_array = dynamic_char_array_create(8);
    t->within_word = false;
    t->first = NULL;
    t->last = NULL;

    return t;
}

void tokenizer_clear(struct tokenizer *t)
{
    t->quote_mode_enabled = false;
    t->tmp_char_array->len = 0;
    t->within_word = false;
    word_item_destroy(t->first);
    t->first = t->last = NULL;
}

void tokenizer_accept_char(struct tokenizer *t, char c)
{
    if (c == '\\' && !t->char_escape_mode_enabled) {
        t->char_escape_mode_enabled = true;
    } else if (c == '"' && !t->char_escape_mode_enabled) {
        t->quote_mode_enabled = !t->quote_mode_enabled;

        if (!t->quote_mode_enabled && t->tmp_char_array->len == 0) {
            t->within_word = true;
        }
    } else if ((c == ' ' || c == '\t' || c == '\n') && t->quote_mode_enabled == false) {
        if (t->within_word == true) {
            struct word_item *wi;
            char *word = malloc(t->tmp_char_array->len + 1);
            memcpy(word, t->tmp_char_array->ptr, t->tmp_char_array->len);
            word[t->tmp_char_array->len] = '\0';
            wi = word_item_create(word);
            word_item_push(&t->first, &t->last, wi);
            t->tmp_char_array->len = 0;
        }

        t->within_word = false;
    } else if ((c != '\\' && c != '"') || t->char_escape_mode_enabled) {
        dynamic_char_array_append(t->tmp_char_array, c);
        
        t->within_word = true;
        t->char_escape_mode_enabled = false;
    }
}

void tokenizer_destroy(struct tokenizer *t)
{
    dynamic_char_array_destroy(t->tmp_char_array);
    free(t);
}
