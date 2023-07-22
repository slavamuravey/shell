#ifndef VM_SENTRY
#define VM_SENTRY

#include "ast.h"

struct vm {
};

struct vm *vm_create();
void vm_run(struct vm *vm, struct ast *ast);
void vm_destroy(struct vm *vm);

#endif