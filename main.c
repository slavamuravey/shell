#include "shell.h"

int main()
{
    struct tokenizer *t = tokenizer_create();
    struct parser *p = parser_create();
    struct shell *s = shell_create(t, p);
    shell_run(s);
    tokenizer_destroy(t);
    parser_destroy(p);
    shell_destroy(s);

    return 0;
}
