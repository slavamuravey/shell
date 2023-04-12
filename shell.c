#include <stdlib.h>
#include <stdio.h>
#include "shell.h"

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
    int c;
    shell_print_input_prompt(s);

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
