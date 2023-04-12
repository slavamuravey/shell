#ifndef SHELL_SENTRY
#define SHELL_SENTRY

#include "tokenizer.h"

struct shell {
    struct tokenizer *t;
};

struct shell *shell_create(struct tokenizer *t);
void shell_print_input_prompt(struct shell *s);
void shell_exec(struct shell *s);
void shell_run(struct shell *s);
void shell_destroy(struct shell *s);

#endif
