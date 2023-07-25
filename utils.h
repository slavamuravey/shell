#ifndef UTILS_SENTRY
#define UTILS_SENTRY

#include <unistd.h>
#include <sys/wait.h>

char *create_string_from_array(const char *src, size_t len);
char *dupstr(const char *str);
void print_process_exit_status(pid_t pid, int status);

#endif
