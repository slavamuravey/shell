#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include "shell.h"
#include "utils.h"
#include "token.h"

struct shell *shell_create(struct tokenizer *t, struct parser *p)
{
    struct shell *s = malloc(sizeof(struct shell));
    s->t = t;
    s->p = p;

    return s;
}

static void shell_print_input_prompt(struct shell *s)
{
    if (isatty(STDIN_FILENO)) {
        printf("> ");
        fflush(stdout);
    }
}

static char **shell_create_cmd(struct shell *s, struct dynamic_array *tokens)
{
    char **array;
    int i;
    struct token **data;

    if (tokens->len == 0) {
        return NULL;
    }

    array = malloc((tokens->len + 1) * sizeof(char*));
    data = tokens->ptr;
    
    for (i = 0; i < tokens->len; i++) {
        struct token *token = data[i];
        array[i] = token->text;
    }

    array[i] = NULL;

    return array;
}

static void shell_exec_parse(struct shell *s, struct dynamic_array *tokens)
{
    pid_t pid;
    char **cmd;
    struct token **data;
    struct token *token1;

    if (tokens->len == 0) {
        return;
    }

    data = tokens->ptr;
    token1 = data[0];

    if (!strcmp(token1->text, "cd")) {
        char *dir;
        if (tokens->len > 1) {
            struct token *token2 = data[1];
            dir = token2->text;
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

    cmd = shell_create_cmd(s, tokens);

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
    struct tokenize_data *t_data = NULL;
    struct tokenize_error *t_error = NULL;
    struct parse_data *p_data = NULL;
    struct parse_error *p_error = NULL;
    struct dynamic_array *tokens;
    struct ast *ast;

    tokenizer_tokenize(s->t, str, &t_data, &t_error);
    if (t_error) {
        char *message = t_error->message;
        tokens_destroy(t_data->tokens);
        free(t_data);
        free(t_error);
        printf("Tokenize error: %s\n", message);

        return;
    }
    
    tokens = t_data->tokens;
    free(t_data);
    free(t_error);

    parser_parse(s->p, tokens, &p_data, &p_error);
    if (p_error) {
        char *message = p_error->message;
        ast_destroy(p_data->ast);
        free(p_data);
        free(p_error);
        printf("Parse error: %s\n", message);

        return;
    }

    ast = p_data->ast;
    free(p_data);
    free(p_error);

    /*
    * TODO: Replace to VM execution
    */
    shell_exec_parse(s, tokens);

    tokens_destroy(tokens);
    ast_destroy(ast);
}

void shell_run(struct shell *s)
{
    int c;
    struct dynamic_array *chars = dynamic_array_create(4, sizeof(char));

    shell_print_input_prompt(s);
    while ((c = getchar()) != EOF) {
        dynamic_array_append(chars, &c);
        if (c == '\n') {
            char *str = create_string_from_array(chars->ptr, chars->len);
            shell_exec(s, str);
            chars->len = 0;
            free(str);
            shell_print_input_prompt(s);
        }
    }

    free(chars->ptr);
    free(chars);

    if (isatty(STDIN_FILENO)) {
        putchar('\n');
    }
}

void shell_destroy(struct shell *s)
{
    free(s);
}
