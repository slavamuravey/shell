#include <stdlib.h>
#include <stdio.h>
#include "vm.h"

/* static char **shell_create_cmd(struct shell *s, struct token **tokens, size_t len)
{
    char **array;
    int i;

    array = malloc((len + 1) * sizeof(char*));
    
    for (i = 0; i < len; i++) {
        struct token *token = tokens[i];
        if (token->type != TOKEN_TYPE_WORD) {
            break;
        }
        array[i] = token->text;
    }

    array[i] = NULL;

    return array;
}

static void process_cmd(struct shell *s, struct token **tokens, size_t len)
{
    pid_t pid;
    char **cmd;

    if (len == 0) {     
        return;
    }

    if (!strcmp(tokens[0]->text, "cd")) {
        char *dir;
        if (len > 1 && tokens[1]->type != TOKEN_TYPE_EXPRESSION_SEPARATOR_2) {
            dir = tokens[1]->text;
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

    cmd = shell_create_cmd(s, tokens, len);
    if (!*cmd) {
        free(cmd);
        
        return;
    }

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

static void shell_exec_parse(struct shell *s, struct dynamic_array *tokens)
{
    struct token **tokens_ptr_base;
    struct token **tokens_ptr;
    int i;

    if (tokens->len == 0) {
        return;
    }

    tokens_ptr_base = tokens->ptr;
    tokens_ptr = tokens->ptr;
    for (i = 0; i < tokens->len; i++) {
        if ((*tokens_ptr)->type == TOKEN_TYPE_EXPRESSION_SEPARATOR_2) {
            process_cmd(s, tokens_ptr_base, tokens_ptr - tokens_ptr_base);
            tokens_ptr_base = tokens_ptr + 1;
        }
        tokens_ptr++;
    }
} */

struct vm *vm_create()
{
    struct vm *vm = malloc(sizeof(struct vm));

    return vm;
}

void vm_run(struct vm *vm, struct ast *ast)
{
    printf("vm run\n");
}

void vm_destroy(struct vm *vm)
{
    free(vm);
}
