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

char *dupstr(const char *str)
{
    char *res;
    size_t len = strlen(str);
    res = malloc(len + 1);
    strcpy(res, str);
    res[len] = '\0';

    return res;
}
