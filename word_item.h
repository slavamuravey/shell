#ifndef WORD_ITEM_SENTRY
#define WORD_ITEM_SENTRY

struct word_item {
    char *word;
    struct word_item *next;   
};

struct word_item *word_item_create(char *word);
void word_item_push(struct word_item **first, struct word_item **last, struct word_item *new_item);
void word_item_destroy(struct word_item *first);

#endif
