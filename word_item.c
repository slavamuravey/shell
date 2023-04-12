#include <stdlib.h>
#include "word_item.h"

struct word_item *word_item_create(char *word)
{
    struct word_item *wi = malloc(sizeof(struct word_item));
    wi->word = word;
    wi->next = NULL;

    return wi;
}

void word_item_push(struct word_item **first, struct word_item **last, struct word_item *new_item)
{
    if (*last) {
        (*last)->next = new_item;
        *last = (*last)->next;
    } else {
        *first = *last = new_item;
    }
}

void word_item_destroy(struct word_item *first)
{
    struct word_item *tmp;
    while (first) {
        tmp = first;
        first = first->next;
        free(tmp->word);
        free(tmp);
    }
}
