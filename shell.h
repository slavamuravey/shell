#ifndef SHELL_SENTRY
#define SHELL_SENTRY

#include "tokenizer.h"
#include "parser.h"

struct shell {
    struct tokenizer *t;
    struct parser *p;
    struct vm *vm;
};

struct shell *shell_create();
void shell_run(struct shell *s);
void shell_destroy(struct shell *s);

#endif
