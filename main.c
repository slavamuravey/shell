#include "tokenizer.h"
#include "shell.h"

int main()
{
    struct tokenizer *t = tokenizer_create();
    struct shell *s = shell_create(t);
    shell_run(s);
    tokenizer_destroy(t);
    shell_destroy(s);

    return 0;
}
