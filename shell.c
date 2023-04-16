#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include "shell.h"

struct shell *shell_create(struct tokenizer *t)
{
    struct shell *s = malloc(sizeof(struct shell));
    s->t = t;

    return s;
}

static void shell_print_input_prompt(struct shell *s)
{
    if (isatty(STDIN_FILENO)) {
        printf("> ");
    }
}

static char **shell_create_cmd(struct shell *s)
{
    size_t list_len = 0;
    struct word_item *tmp;
    char **array;
    int i = 0;

    for (tmp = s->t->first; tmp; tmp = tmp->next) {
        list_len++;
    }

    if (list_len == 0) {
        return NULL;
    }

    array = malloc((list_len + 1) * sizeof(char*));

    for (tmp = s->t->first; tmp; tmp = tmp->next, i++) {
        array[i] = tmp->word;
    }

    array[i] = NULL;

    return array;
}

static void shell_exec(struct shell *s)
{
    pid_t pid;
    if (s->t->first == NULL) {
        return;
    }

    if (!strcmp(s->t->first->word, "cd")) {
        char *dir;
        if (s->t->first->next != NULL) {
            dir = s->t->first->next->word;
        } else {
            char *home_dir = getenv("HOME");
            if (home_dir == NULL) {
                printf("I don't know where is your home.\n");
                return;
            }
            dir = home_dir;
        }

        if (chdir(dir) == -1) {
            perror(dir);
        }

        return;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        char **cmd = shell_create_cmd(s);
        execvp(*cmd, cmd);
        perror(*cmd);
        exit(1);
    }

    wait(NULL);
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
