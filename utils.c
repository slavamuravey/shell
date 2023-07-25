#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "utils.h"

char *create_string_from_array(const char *src, size_t len)
{
    char *str = malloc(len + 1);
    memcpy(str, src, len);
    str[len] = '\0';

    return str;
}

char *dupstr(const char *str)
{
    char *res;
    size_t len = strlen(str);
    res = malloc(len + 1);
    strcpy(res, str);
    res[len] = '\0';

    return res;
}

void print_process_exit_status(pid_t pid, int status)
{
    char message[100];
    int save_errno;
    if (WIFEXITED(status)) {
        sprintf(message, "process %d exited with code %d\n", pid, WEXITSTATUS(status));
    } else {
        sprintf(message, "process %d killed with signal %d\n", pid, WTERMSIG(status));
    }
    save_errno = errno;
    write(STDOUT_FILENO, message, strlen(message));
    errno = save_errno;
}
