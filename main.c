#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

struct dynamic_char_array {
    size_t len;
    size_t cap;
    char *ptr;
};

struct dynamic_char_array *dynamic_char_array_create(size_t cap)
{
    struct dynamic_char_array *dca = malloc(sizeof(struct dynamic_char_array));
    dca->len = 0;
    dca->cap = cap;
    dca->ptr = malloc(sizeof(char) * cap);

    return dca;
}

void dynamic_char_array_append(struct dynamic_char_array *dca, char c)
{
    dca->len++;

    if (dca->len > dca->cap) {
        dca->cap *= 2;
        dca->ptr = realloc(dca->ptr, dca->cap);
    }

    dca->ptr[dca->len - 1] = c;
}

void dynamic_char_array_destroy(struct dynamic_char_array *dca)
{
    free(dca->ptr);
    free(dca);
}

struct word_item {
    char *word;
    struct word_item *next;   
};

struct word_item *word_item_create(char *word)
{
    struct word_item *wi = malloc(sizeof(struct word_item));
    wi->word = word;
    wi->next = NULL;

    return wi;
}

void word_item_push(struct word_item **first, struct word_item **last, struct word_item *new)
{
    if (*last) {
        (*last)->next = new;
        *last = (*last)->next;
    } else {
        *first = *last = new;
    }
}

void word_item_destroy(struct word_item *first)
{
    struct word_item *tmp;
    while (first) {
        tmp = first;
        first = first->next;
        free(tmp->word);
        free(tmp);
    }
}

struct tokenizer {
    bool quote_mode_enabled;
    bool char_escape_mode_enabled;
    struct dynamic_char_array *tmp_char_array;
    bool within_word;
    struct word_item *first;
    struct word_item *last;
};

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
            char *word = malloc(t->tmp_char_array->len + 1);
            memcpy(word, t->tmp_char_array->ptr, t->tmp_char_array->len);
            word[t->tmp_char_array->len] = '\0';
            struct word_item *wi = word_item_create(word);
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

struct shell {
    struct tokenizer *t;
};

struct shell *shell_create(struct tokenizer *t)
{
    struct shell *s = malloc(sizeof(struct shell));
    s->t = t;

    return s;
}

void shell_print_input_prompt(struct shell *s)
{
    if (isatty(STDIN_FILENO)) {
        printf("> ");
    }
}

void shell_exec(struct shell *s)
{
    struct word_item *tmp;
    for (tmp = s->t->first; tmp; tmp = tmp->next) {
        printf("[%s]\n", tmp->word);
    }
}

void shell_run(struct shell *s)
{
    shell_print_input_prompt(s);
    
    int c;
    while ((c = getchar()) != EOF) {
        tokenizer_accept_char(s->t, c);

        if (c == '\n') {
            if (s->t->quote_mode_enabled == true) {
                printf("Error: unmatched quotes\n");
            } else {
                shell_exec(s);
            }

            tokenizer_clear(s->t);
            shell_print_input_prompt(s);
        }
    }

    if (isatty(STDIN_FILENO)) {
        putchar('\n');
    }
}

void shell_destroy(struct shell *s)
{
    free(s);
}

int main()
{
    struct tokenizer *t = tokenizer_create();
    struct shell *s = shell_create(t);
    shell_run(s);
    tokenizer_destroy(t);
    shell_destroy(s);

    return 0;
}
