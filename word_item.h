#ifndef WORD_ITEM_SENTRY
#define WORD_ITEM_SENTRY

#include <stdbool.h>

struct word_item {
    char *word;
    bool separator;
    struct word_item *next;   
};

struct word_item *word_item_create(char *word, bool separator);
void word_item_push(struct word_item **first, struct word_item **last, struct word_item *new_item);
void word_item_destroy(struct word_item *first);

#endif
