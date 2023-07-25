#ifndef VM_SENTRY
#define VM_SENTRY

#include "ast.h"

struct vm {
    const struct ast *ast;
    int status;
    int *left_pipe;
    int *right_pipe;
    struct dynamic_array *waiting_pids_array;
    struct dynamic_array *waited_pids_array;
};

struct vm *vm_create(const struct ast *ast);
void vm_run(struct vm *vm);
void vm_destroy(struct vm *vm);

#endif