#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include "shell.h"
#include "utils.h"

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

static char **shell_create_cmd(struct shell *s, struct token *token_first)
{
    size_t list_len = 0;
    struct token *tmp;
    char **array;
    int i = 0;

    for (tmp = token_first; tmp; tmp = tmp->next) {
        list_len++;
    }

    if (list_len == 0) {
        return NULL;
    }

    array = malloc((list_len + 1) * sizeof(char*));

    for (tmp = token_first; tmp; tmp = tmp->next, i++) {
        array[i] = tmp->token;
    }

    array[i] = NULL;

    return array;
}

static void shell_exec_parse(struct shell *s, struct token *token_first)
{
    pid_t pid;
    char **cmd;

    if (!token_first) {
        return;
    }

    if (!strcmp(token_first->token, "cd")) {
        char *dir;
        if (token_first->next) {
            dir = token_first->next->token;
        } else {
            char *home_dir = getenv("HOME");
            if (!home_dir) {
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

    cmd = shell_create_cmd(s, token_first);

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        execvp(*cmd, cmd);
        perror(*cmd);
        exit(1);
    }

    free(cmd);

    wait(NULL);
}

static void shell_exec(struct shell *s, const char *str)
{
    struct tokenize_data *data = NULL;
    struct tokenize_error *error = NULL;
    struct token *token_first;
    tokenizer_tokenize(s->t, str, &data, &error);
    if (error) {
        char *message = error->message;
        token_destroy(data->token_first);
        free(data);
        free(error);
        printf("Error: %s\n", message);

        return;
    }
    
    token_first = data->token_first;
    free(data);
    free(error);
    shell_exec_parse(s, token_first);
    token_destroy(token_first);
}

void shell_run(struct shell *s)
{
    int c;
    struct dynamic_char_array *dca = dynamic_char_array_create(4);

    shell_print_input_prompt(s);
    while ((c = getchar()) != EOF) {
        dynamic_char_array_append(dca, c);
        if (c == '\n') {
            char *str = create_string_from_array(dca->ptr, dca->len);
            shell_exec(s, str);
            dca->len = 0;
            free(str);
            shell_print_input_prompt(s);
        }
    }

    dynamic_char_array_destroy(dca);

    if (isatty(STDIN_FILENO)) {
        putchar('\n');
    }
}

void shell_destroy(struct shell *s)
{
    free(s);
}
