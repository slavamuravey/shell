#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "shell.h"
#include "utils.h"
#include "token.h"
#include "tokenizer.h"
#include "parser.h"
#include "vm.h"

static void sigchld_handler(int s)
{   
    pid_t pid;
    int status;
    signal(SIGCHLD, sigchld_handler);
    while (true) {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid <= 0) {
            break;
        }

        /* print_process_exit_status(pid, status); */
    }
}

struct shell *shell_create()
{
    struct shell *s = malloc(sizeof(struct shell));
    struct tokenizer *t = tokenizer_create();
    struct parser *p = parser_create();
    s->t = t;
    s->p = p;

    return s;
}

static bool isrepl()
{
    return isatty(STDIN_FILENO);
}

static void shell_print_input_prompt(struct shell *s)
{
    if (isrepl()) {
        printf("> ");
        fflush(stdout);
    }
}

static void shell_exec(struct shell *s, const char *str)
{
    struct tokenize_data *t_data = NULL;
    struct tokenize_error *t_error = NULL;
    struct parse_data *p_data = NULL;
    struct parse_error *p_error = NULL;
    struct dynamic_array *tokens;
    struct ast *ast;
    struct vm *vm;

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
        tokens_destroy(tokens);
        ast_destroy(p_data->ast);
        free(p_data);
        free(p_error);
        printf("Parse error: %s\n", message);

        return;
    }

    ast = p_data->ast;
    free(p_data);
    free(p_error);

    vm = vm_create(ast);
    vm_run(vm);
    vm_destroy(vm);

    tokens_destroy(tokens);
    ast_destroy(ast);
}

void shell_run(struct shell *s)
{
    int c;
    struct dynamic_array *chars = dynamic_array_create(4, sizeof(char));
    signal(SIGCHLD, sigchld_handler);

    shell_print_input_prompt(s);
    while (true) {
        int save_errno = errno;
        c = getchar();
        if (c == EOF) {
            if (errno == EINTR) {
                errno = save_errno;

                continue;
            }

            break;
        }

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
    tokenizer_destroy(s->t);
    parser_destroy(s->p);
    free(s);
}
