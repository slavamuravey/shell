#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

char *create_string_from_array(const char *src, size_t len)
{
    char *str = malloc(len + 1);
    memcpy(str, src, len);
    str[len] = '\0';

    return str;
}

void append_char_to_string(char *str, size_t *buffer, char c)
{
    size_t len = strlen(str);
    if (len + 1 == *buffer) {
        *buffer *= 2; 
        str = realloc(str, *buffer);
    }

    str[len] = c;
    str[len + 1] = '\0';
}
