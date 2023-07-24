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

static int vm_run_script(struct vm *vm, struct ast *ast)
{
    int i;
    struct dynamic_array *expressions_array = ast->data.script.expressions;
    struct ast **expressions = expressions_array->ptr;
    int status = 0;

    for (i = 0; i < expressions_array->len; i++) {
        status = vm_run(vm, expressions[i]);
    }

    return status;
}

static void vm_run_command(struct vm *vm, struct ast *ast, struct dynamic_array *pids_array)
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
    dynamic_array_append(pids_array, &pid);

    free(cmd);
}

static void vm_run_pipeline(struct vm *vm, struct ast *ast)
{

}

static int status_to_exit_code(int status)
{
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

static int vm_run_logical_expression_operands(
    struct vm *vm, 
    struct ast *left, 
    struct ast *right, 
    enum ast_data_logical_expression_type type
)
{
    int left_status = vm_run(vm, left);
    int status = left_status;
    switch (type) {
        case AST_DATA_LOGICAL_EXPRESSION_TYPE_AND:
            if (!left_status) {
                status = vm_run(vm, right);
            }

            break;
        case AST_DATA_LOGICAL_EXPRESSION_TYPE_OR:
            if (left_status) {
                status = vm_run(vm, right);
            }

            break;
    }

    return status;
}

static int vm_run_logical_expression(struct vm *vm, struct ast *ast)
{
    struct ast *left = ast->data.logical_expression.left;
    struct ast *right = ast->data.logical_expression.right;
    enum ast_data_logical_expression_type type = ast->data.logical_expression.type;
    int status = 0;

    if (ast->async) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            exit(status_to_exit_code(vm_run_logical_expression_operands(
                vm,
                left,
                right,
                type
            )));
        }
    } else {
        status = vm_run_logical_expression_operands(
            vm,
            left,
            right,
            type
        );
    }

    return status;
}

static void vm_run_subshell(struct vm *vm, struct ast *ast, struct dynamic_array *pids_array)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        exit(status_to_exit_code(vm_run(vm, ast->data.subshell.script)));
    } else {
        dynamic_array_append(pids_array, &pid);
    }
}

bool pid_in_array(struct dynamic_array *pids_array, pid_t pid)
{
    pid_t *pids = pids_array->ptr;
    int i;
    for (i = 0; i < pids_array->len; i++) {
        if (pids[i] == pid) {
            return true;
        }
    }

    return false;
}

int vm_run(struct vm *vm, struct ast *ast)
{
    struct dynamic_array *waiting_pids_array = dynamic_array_create(4, sizeof(pid_t));
    struct dynamic_array *waited_pids_array = dynamic_array_create(4, sizeof(pid_t));
    int status = 0;

    if (!ast) {
        return status;
    }

    switch (ast->type) {
        case AST_TYPE_SCRIPT:
            status = vm_run_script(vm, ast);
            break;
        case AST_TYPE_COMMAND:
            vm_run_command(vm, ast, waiting_pids_array);
            break;
        case AST_TYPE_PIPELINE:
            vm_run_pipeline(vm, ast);
            break;
        case AST_TYPE_LOGICAL_EXPRESSION:
            status = vm_run_logical_expression(vm, ast);
            break;
        case AST_TYPE_SUBSHELL:
            vm_run_subshell(vm, ast, waiting_pids_array);
            break;
    }

    if (!ast->async && waiting_pids_array->len) {
        pid_t waited_pid;
        signal(SIGCHLD, SIG_DFL);
        while (true) {
            waited_pid = wait(&status);
            if (pid_in_array(waiting_pids_array, waited_pid)) {
                dynamic_array_append(waited_pids_array, &waited_pid);
            }
            if (waiting_pids_array->len == waited_pids_array->len) {
                break;
            }
            print_process_exit_status(waited_pid, status);
        }
        signal(SIGCHLD, sigchld_handler);
    }

    return status;
}

void vm_destroy(struct vm *vm)
{
    signal(SIGCHLD, SIG_DFL);
    free(vm);
}
