#include "shell.h"

int main()
{
    struct shell *s = shell_create();
    shell_run(s);
    shell_destroy(s);

    return 0;
}
