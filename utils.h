#ifndef UTILS_SENTRY
#define UTILS_SENTRY

#include <unistd.h>

char *create_string_from_array(const char *src, size_t len);
void append_char_to_string(char *str, size_t *buffer, char c);

#endif
