#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "vm.h"
#include "dynamic_array.h"
#include "ast.h"

struct vm *vm_create()
{
    struct vm *vm = malloc(sizeof(struct vm));

    return vm;
}

static void vm_run_script(struct vm *vm, struct ast *ast)
{
    int i;
    struct dynamic_array *expressions_array = ast->data.script.expressions;
    struct ast **expressions = expressions_array->ptr;

    for (i = 0; i < expressions_array->len; i++) {
        vm_run(vm, expressions[i]);
    }
}

static void vm_run_command(struct vm *vm, struct ast *ast)
{
    pid_t pid;
    char **cmd;
    struct dynamic_array *words_array = ast->data.command.words;
    char **words = words_array->ptr;
    size_t words_len = words_array->len;

    if (words_len == 0) {     
        return;
    }

    if (!strcmp(words[0], "cd")) {
        char *dir;
        if (words_len > 1) {
            dir = words[1];
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

    size_t size = sizeof(char*);
    cmd = malloc(size * (words_len + 1));
    memcpy(cmd, words, words_len * size);
    cmd[words_len] = NULL;

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

static void vm_run_pipeline(struct vm *vm, struct ast *ast)
{

}

static void vm_run_logical_expression(struct vm *vm, struct ast *ast)
{

}

static void vm_run_subshell(struct vm *vm, struct ast *ast)
{

}

void vm_run(struct vm *vm, struct ast *ast)
{
    if (!ast) {
        return;
    }

    switch (ast->type) {
        case AST_TYPE_SCRIPT:
            vm_run_script(vm, ast);
            break;
        case AST_TYPE_COMMAND:
            vm_run_command(vm, ast);
            break;
        case AST_TYPE_PIPELINE:
            vm_run_pipeline(vm, ast);
            break;
        case AST_TYPE_LOGICAL_EXPRESSION:
            vm_run_logical_expression(vm, ast);
            break;
        case AST_TYPE_SUBSHELL:
            vm_run_subshell(vm, ast);
            break;
    }
}

void vm_destroy(struct vm *vm)
{
    free(vm);
}
