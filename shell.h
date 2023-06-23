#ifndef SHELL_SENTRY
#define SHELL_SENTRY

#include "tokenizer.h"
#include "parser.h"

struct shell {
    struct tokenizer *t;
    struct parser *p;
};

struct shell *shell_create(struct tokenizer *t, struct parser *p);
void shell_run(struct shell *s);
void shell_destroy(struct shell *s);

#endif
