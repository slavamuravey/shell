#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

char *create_string_from_array(const char *src, size_t len)
{
    char *word = malloc(len + 1);
    memcpy(word, src, len);
    word[len] = '\0';

    return word;
}
