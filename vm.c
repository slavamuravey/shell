#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "vm.h"
#include "dynamic_array.h"
#include "ast.h"

static void print_process_exit_status(pid_t pid, int status)
{
    char message[100];
    int save_errno;
    if (WIFEXITED(status)) {
        sprintf(message, "process %d exited with code %d\n", pid, WEXITSTATUS(status));
    } else {
        sprintf(message, "process %d killed with signal %d\n", pid, WTERMSIG(status));
    }
    save_errno = errno;
    write(STDOUT_FILENO, message, strlen(message));
    errno = save_errno;
}

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

        print_process_exit_status(pid, status);
    }
}

struct vm *vm_create()
{
    struct vm *vm = malloc(sizeof(struct vm));
    signal(SIGCHLD, sigchld_handler);

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
    size_t size;

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

    size = sizeof(char*);
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

    if (!ast->async) {
        pid_t awaited_pid;
        signal(SIGCHLD, SIG_DFL);
        while (true) {
            int status;
            awaited_pid = wait(&status);
            if (awaited_pid == pid) {
                break;
            }
            print_process_exit_status(awaited_pid, status);
        }
        signal(SIGCHLD, sigchld_handler);
    }
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
    signal(SIGCHLD, SIG_DFL);
    free(vm);
}
