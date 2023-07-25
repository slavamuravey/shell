#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "vm.h"
#include "dynamic_array.h"
#include "ast.h"
#include "utils.h"

struct vm *vm_create(const struct ast *ast)
{
    struct vm *vm = malloc(sizeof(struct vm));
    vm->ast = ast;
    vm->waiting_pids_array = dynamic_array_create(4, sizeof(pid_t));
    vm->waited_pids_array = dynamic_array_create(4, sizeof(pid_t));
    vm->status = 0;
    vm->left_pipe = NULL;
    vm->right_pipe = NULL;

    return vm;
}

static void vm_run_script(struct vm *vm)
{
    int i;
    struct dynamic_array *expressions_array = vm->ast->data.script.expressions;
    struct ast **expressions = expressions_array->ptr;
    vm->status = 0;

    for (i = 0; i < expressions_array->len; i++) {
        struct vm *sub_vm = vm_create(expressions[i]);
        vm_run(sub_vm);
        vm->status = sub_vm->status;
        vm_destroy(sub_vm);
    }
}

static void vm_run_command(struct vm *vm)
{
    pid_t pid;
    char **cmd;
    struct dynamic_array *words_array = vm->ast->data.command.words;
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
        if (vm->left_pipe) {
            dup2(vm->left_pipe[0], STDIN_FILENO);
            close(vm->left_pipe[0]);
            close(vm->left_pipe[1]);
        }
        if (vm->right_pipe) {
            dup2(vm->right_pipe[1], STDOUT_FILENO);
            close(vm->right_pipe[0]);
            close(vm->right_pipe[1]);
        }

        execvp(*cmd, cmd);
        perror(*cmd);
        exit(1);
    }
    dynamic_array_append(vm->waiting_pids_array, &pid);

    free(cmd);
}

static void vm_run_pipeline(struct vm *vm)
{
    int i;
    int prev_pipe[2];
    int cur_pipe[2];
    struct dynamic_array *expressions_array = vm->ast->data.pipeline.expressions;
    struct ast **expressions = expressions_array->ptr;
    for (i = 0; i < expressions_array->len; i++) {
        struct vm *sub_vm;
        int j;
        pid_t *pids;
        if (i < expressions_array->len - 1) {
            pipe(cur_pipe);
        }

        sub_vm = vm_create(expressions[i]);
        sub_vm->left_pipe = i == 0 ? NULL : prev_pipe;
        sub_vm->right_pipe = i == expressions_array->len - 1 ? NULL : cur_pipe;
        vm_run(sub_vm);
        pids = sub_vm->waiting_pids_array->ptr;
        for (j = 0; j < sub_vm->waiting_pids_array->len; j++) {
            dynamic_array_append(vm->waiting_pids_array, pids + j);
        }
        vm_destroy(sub_vm);

        if (i > 0) {
            close(prev_pipe[0]);
            close(prev_pipe[1]);
        }

        memcpy(prev_pipe, cur_pipe, 2 * sizeof(int));
    }
}

static int status_to_exit_code(int status)
{
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

static void vm_run_logical_expression_operands(
    struct vm *vm, 
    struct ast *left, 
    struct ast *right, 
    enum ast_data_logical_expression_type type
)
{
    int left_status;
    int status;
    struct vm *sub_vm = vm_create(left);
    vm_run(sub_vm);
    left_status = sub_vm->status;
    vm_destroy(sub_vm);
    status = left_status;
    switch (type) {
        case AST_DATA_LOGICAL_EXPRESSION_TYPE_AND:
            if (!left_status) {
                struct vm *sub_vm = vm_create(right);
                vm_run(sub_vm);
                status = sub_vm->status;
                vm_destroy(sub_vm);
            }

            break;
        case AST_DATA_LOGICAL_EXPRESSION_TYPE_OR:
            if (left_status) {
                struct vm *sub_vm = vm_create(right);
                vm_run(sub_vm);
                status = sub_vm->status;
                vm_destroy(sub_vm);
            }

            break;
    }

    vm->status = status;
}

static void vm_run_logical_expression(struct vm *vm)
{
    struct ast *left = vm->ast->data.logical_expression.left;
    struct ast *right = vm->ast->data.logical_expression.right;
    enum ast_data_logical_expression_type type = vm->ast->data.logical_expression.type;

    if (vm->ast->async) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            vm_run_logical_expression_operands(
                vm,
                left,
                right,
                type
            );
            exit(status_to_exit_code(vm->status));
        }
    } else {
        vm_run_logical_expression_operands(
            vm,
            left,
            right,
            type
        );
    }
}

static void vm_run_subshell(struct vm *vm)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        struct vm *sub_vm;
        if (vm->left_pipe) {
            dup2(vm->left_pipe[0], STDIN_FILENO);
            close(vm->left_pipe[0]);
            close(vm->left_pipe[1]);
        }
        if (vm->right_pipe) {
            dup2(vm->right_pipe[1], STDOUT_FILENO);
            close(vm->right_pipe[0]);
            close(vm->right_pipe[1]);
        }

        sub_vm = vm_create(vm->ast->data.subshell.script);
        vm_run(sub_vm);
        exit(status_to_exit_code(sub_vm->status));
    } else {
        dynamic_array_append(vm->waiting_pids_array, &pid);
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

void vm_run(struct vm *vm)
{
    vm->waiting_pids_array->len = 0;
    vm->waited_pids_array->len = 0;

    if (!vm->ast) {
        return;
    }

    switch (vm->ast->type) {
        case AST_TYPE_SCRIPT:
            vm_run_script(vm);
            break;
        case AST_TYPE_COMMAND:
            vm_run_command(vm);
            break;
        case AST_TYPE_PIPELINE:
            vm_run_pipeline(vm);
            break;
        case AST_TYPE_LOGICAL_EXPRESSION:
            vm_run_logical_expression(vm);
            break;
        case AST_TYPE_SUBSHELL:
            vm_run_subshell(vm);
            break;
    }

    if (!vm->ast->async && vm->waiting_pids_array->len) {
        pid_t waited_pid;
        __sighandler_t prev_sig_handler = signal(SIGCHLD, SIG_DFL);
        while (true) {
            int status;
            waited_pid = wait(&status);
            vm->status = status;
            if (pid_in_array(vm->waiting_pids_array, waited_pid)) {
                /* print_process_exit_status(waited_pid, status); */
                dynamic_array_append(vm->waited_pids_array, &waited_pid);
            }
            
            if (vm->waiting_pids_array->len == vm->waited_pids_array->len) {
                break;
            }
        }
        signal(SIGCHLD, prev_sig_handler);
    }
}

void vm_destroy(struct vm *vm)
{
    free(vm->waiting_pids_array->ptr);
    free(vm->waiting_pids_array);
    free(vm->waited_pids_array->ptr);
    free(vm->waited_pids_array);
    free(vm);
}
