#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tokenizer.h"
#include "utils.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define TOKEN_AMP "&"
#define TOKEN_AMP_2 "&&"
#define TOKEN_PIPE "|"
#define TOKEN_PIPE_2 "||"
#define TOKEN_OPENING_TRIANGLE_BRACKET "<"
#define TOKEN_CLOSING_TRIANGLE_BRACKET ">"
#define TOKEN_CLOSING_TRIANGLE_BRACKET_2 ">>"
#define TOKEN_OPENING_PARENTHESIS "("
#define TOKEN_CLOSING_PARENTHESIS ")"
#define TOKEN_SEMICOLON ";"

/**
 * The order of the separators matters. Longest separators first.
*/
static const char *const SEPARATOR_TOKENS[] = {
    TOKEN_AMP_2,
    TOKEN_PIPE_2,
    TOKEN_CLOSING_TRIANGLE_BRACKET_2,
    TOKEN_AMP,
    TOKEN_PIPE,
    TOKEN_OPENING_TRIANGLE_BRACKET,
    TOKEN_CLOSING_TRIANGLE_BRACKET,
    TOKEN_OPENING_PARENTHESIS,
    TOKEN_CLOSING_PARENTHESIS,
    TOKEN_SEMICOLON
};

struct tokenizer *tokenizer_create()
{
    struct tokenizer *t = malloc(sizeof(struct tokenizer));
    t->quote_mode_enabled = false;
    t->char_escape_mode_enabled = false;
    t->tmp_word = dynamic_char_array_create(8);
    t->tmp_separator = dynamic_char_array_create(8);
    t->within_word = false;
    t->first = NULL;
    t->last = NULL;

    return t;
}

static void tokenizer_add_word_item(struct tokenizer *t, struct dynamic_char_array *array, bool separator)
{
    struct word_item *wi;
    char *word = create_string_from_array(array->ptr, array->len);
    wi = word_item_create(word, separator);
    word_item_push(&t->first, &t->last, wi);
}

void tokenizer_clear(struct tokenizer *t)
{
    t->quote_mode_enabled = false;
    t->tmp_word->len = 0;
    t->tmp_separator->len = 0;
    t->within_word = false;
    word_item_destroy(t->first);
    t->first = t->last = NULL;
}

static void tokenizer_handle_separator_token(struct tokenizer *t, char c)
{
    int i;
    size_t tokens_count = sizeof(SEPARATOR_TOKENS) / sizeof(*SEPARATOR_TOKENS);

    if (t->quote_mode_enabled) {
        return;
    }

    do {
        dynamic_char_array_append(t->tmp_separator, c);
        
        for (i = 0; i < tokens_count; i++) {
            const char *token = SEPARATOR_TOKENS[i];
            size_t token_len = strlen(token);

            if (!memcmp(t->tmp_separator->ptr, token, MIN(token_len, t->tmp_separator->len))) {
                if (token_len < t->tmp_separator->len) {
                    t->tmp_word->len -= token_len;
                    t->within_word = false;

                    if (t->tmp_word->len) {
                        tokenizer_add_word_item(t, t->tmp_word, false);
                        t->tmp_word->len = 0;
                    }
                    
                    t->tmp_separator->len--;
                    tokenizer_add_word_item(t, t->tmp_separator, true);
                    t->tmp_separator->len = 0;
                }

                break;
            }
        }
    } while (t->tmp_separator->len == 0); 

    if (i == tokens_count) {
        t->tmp_separator->len = 0;
    }
}

void tokenizer_accept_char(struct tokenizer *t, char c)
{
    tokenizer_handle_separator_token(t, c);

    if (c == '\\' && !t->char_escape_mode_enabled) {
        t->char_escape_mode_enabled = true;
    } else if (c == '"' && !t->char_escape_mode_enabled) {
        t->quote_mode_enabled = !t->quote_mode_enabled;

        if (!t->quote_mode_enabled && t->tmp_word->len == 0) {
            t->within_word = true;
        }
    } else if ((c == ' ' || c == '\t' || c == '\n') && !t->quote_mode_enabled) {
        if (t->within_word) {
            tokenizer_add_word_item(t, t->tmp_word, false);
            t->tmp_word->len = 0;
        }

        t->within_word = false;
    } else if ((c != '\\' && c != '"') || t->char_escape_mode_enabled) {
        dynamic_char_array_append(t->tmp_word, c);
        
        t->within_word = true;
        t->char_escape_mode_enabled = false;
    }
}

void tokenizer_destroy(struct tokenizer *t)
{
    dynamic_char_array_destroy(t->tmp_word);
    dynamic_char_array_destroy(t->tmp_separator);
    free(t);
}
