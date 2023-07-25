#ifndef VM_SENTRY
#define VM_SENTRY

#include "ast.h"

struct data {
    int *left_pipe;
    int *right_pipe;
};

struct vm {
};

struct vm *vm_create();
int vm_run(struct vm *vm, struct ast *ast, const struct data *data);
void vm_destroy(struct vm *vm);

#endif