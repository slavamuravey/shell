#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tokenizer.h"
#include "utils.h"
#include "token.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define TOKEN_TEXT_AMP "&"
#define TOKEN_TEXT_AMP_2 "&&"
#define TOKEN_TEXT_PIPE "|"
#define TOKEN_TEXT_PIPE_2 "||"
#define TOKEN_TEXT_OPENING_TRIANGLE_BRACKET "<"
#define TOKEN_TEXT_CLOSING_TRIANGLE_BRACKET ">"
#define TOKEN_TEXT_CLOSING_TRIANGLE_BRACKET_2 ">>"
#define TOKEN_TEXT_OPENING_PARENTHESIS "("
#define TOKEN_TEXT_CLOSING_PARENTHESIS ")"
#define TOKEN_TEXT_SEMICOLON ";"

/**
 * The order matters. Longest string first.
*/
static const char *const SEPARATOR_TOKENS[] = {
    TOKEN_TEXT_AMP_2,
    TOKEN_TEXT_PIPE_2,
    TOKEN_TEXT_CLOSING_TRIANGLE_BRACKET_2,
    TOKEN_TEXT_AMP,
    TOKEN_TEXT_PIPE,
    TOKEN_TEXT_OPENING_TRIANGLE_BRACKET,
    TOKEN_TEXT_CLOSING_TRIANGLE_BRACKET,
    TOKEN_TEXT_OPENING_PARENTHESIS,
    TOKEN_TEXT_CLOSING_PARENTHESIS,
    TOKEN_TEXT_SEMICOLON
};

static struct tokenize_data *tokenize_data_create(struct dynamic_array *tokens)
{
    struct tokenize_data *data = malloc(sizeof(struct tokenize_data));
    data->tokens = tokens;

    return data;
}

static struct tokenize_error *tokenize_error_create(char *message)
{
    struct tokenize_error *error = malloc(sizeof(struct tokenize_error));
    error->message = message;

    return error;
}

struct tokenizer *tokenizer_create()
{
    struct tokenizer *t = malloc(sizeof(struct tokenizer));
    t->quote_mode_enabled = false;
    t->char_escape_mode_enabled = false;
    t->tmp_word = dynamic_array_create(8, sizeof(char));
    t->tmp_separator = dynamic_array_create(8, sizeof(char));
    t->within_word = false;
    t->tokens = NULL;

    return t;
}

static enum token_type get_token_type_by_token_text(const char *text)
{
    if (!strcmp(text, TOKEN_TEXT_AMP)) {
        return TOKEN_TYPE_ASYNC;
    }

    if (!strcmp(text, TOKEN_TEXT_AMP_2)) {
        return TOKEN_TYPE_AND;
    }

    if (!strcmp(text, TOKEN_TEXT_PIPE)) {
        return TOKEN_TYPE_PIPE;
    }

    if (!strcmp(text, TOKEN_TEXT_PIPE_2)) {
        return TOKEN_TYPE_OR;
    }

    if (!strcmp(text, TOKEN_TEXT_OPENING_TRIANGLE_BRACKET)) {
        return TOKEN_TYPE_REDIRECT_INPUT;
    }

    if (!strcmp(text, TOKEN_TEXT_CLOSING_TRIANGLE_BRACKET)) {
        return TOKEN_TYPE_REDIRECT_OUTPUT;
    }

    if (!strcmp(text, TOKEN_TEXT_CLOSING_TRIANGLE_BRACKET_2)) {
        return TOKEN_TYPE_REDIRECT_OUTPUT_APPEND;
    }

    if (!strcmp(text, TOKEN_TEXT_OPENING_PARENTHESIS)) {
        return TOKEN_TYPE_SUBSHELL_START;
    }

    if (!strcmp(text, TOKEN_TEXT_CLOSING_PARENTHESIS)) {
        return TOKEN_TYPE_SUBSHELL_END;
    }

    if (!strcmp(text, TOKEN_TEXT_SEMICOLON)) {
        return TOKEN_TYPE_COMMAND_SEPARATOR;
    }

    return TOKEN_TYPE_WORD;
}

static void tokenizer_add_token(struct tokenizer *t, struct dynamic_array *array)
{
    char *text = create_string_from_array(array->ptr, array->len);
    struct token *token = token_create(get_token_type_by_token_text(text), text);
    dynamic_array_append(t->tokens, &token);
}

static void tokenizer_reset(struct tokenizer *t)
{
    t->quote_mode_enabled = false;
    t->tmp_word->len = 0;
    t->tmp_separator->len = 0;
    t->within_word = false;
    t->tokens = NULL;
}

static void tokenizer_handle_separator_token(struct tokenizer *t, char c)
{
    int i;
    size_t tokens_count = sizeof(SEPARATOR_TOKENS) / sizeof(*SEPARATOR_TOKENS);

    if (t->quote_mode_enabled) {
        return;
    }

    do {
        dynamic_array_append(t->tmp_separator, &c);
        
        for (i = 0; i < tokens_count; i++) {
            const char *token = SEPARATOR_TOKENS[i];
            size_t token_len = strlen(token);

            if (!memcmp(t->tmp_separator->ptr, token, MIN(token_len, t->tmp_separator->len))) {
                if (token_len < t->tmp_separator->len) {
                    t->tmp_word->len -= token_len;
                    t->within_word = false;

                    if (t->tmp_word->len) {
                        tokenizer_add_token(t, t->tmp_word);
                        t->tmp_word->len = 0;
                    }
                    
                    t->tmp_separator->len--;
                    tokenizer_add_token(t, t->tmp_separator);
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

void tokenizer_tokenize(struct tokenizer *t, const char *str, struct tokenize_data **data, struct tokenize_error **error)
{
    const char *p = str;
    t->tokens = tokens_create();
    
    for (; *p; p++) {
        char c = *p;
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
                tokenizer_add_token(t, t->tmp_word);
                t->tmp_word->len = 0;
            }

            t->within_word = false;
        } else if ((c != '\\' && c != '"') || t->char_escape_mode_enabled) {
            dynamic_array_append(t->tmp_word, &c);
            
            t->within_word = true;
            t->char_escape_mode_enabled = false;
        }
    }

    if (t->quote_mode_enabled) {
        *error = tokenize_error_create("unmatched quotes");
    }

    *data = tokenize_data_create(t->tokens);

    tokenizer_reset(t);
}

void tokenizer_destroy(struct tokenizer *t)
{
    if (t) {
        free(t->tmp_word->ptr);
        free(t->tmp_word);
        
        free(t->tmp_separator->ptr);
        free(t->tmp_separator);
    }
    
    free(t);
}
