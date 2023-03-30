#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

struct word_item {
    char *word;
    struct word_item *next;   
};

void print_input_prompt()
{
    if (isatty(STDIN_FILENO)) {
        printf("> ");
    }
}

void free_word_item_list(struct word_item *first)
{
    struct word_item *tmp;
    while (first) {
        tmp = first;
        first = first->next;
        free(tmp->word);
        free(tmp);
    }
}

void push_word_item(struct word_item **first, struct word_item **last, char *word)
{
    struct word_item *tmp = malloc(sizeof(struct word_item));
    tmp->word = strdup(word);
    tmp->next = NULL;

    if (*last) {
        (*last)->next = tmp;
        *last = (*last)->next;
    } else {
        *first = *last = tmp;
    }
}

int main()
{
    print_input_prompt();
    bool quote_mode_enabled = false;
    bool char_escape_mode_enabled = false;
    char *word = NULL;
    size_t buffer = 8;
    bool within_word = false;
    struct word_item *first = NULL, *last = NULL, *tmp;
    int i = 0;
    int c;
    
    while ((c = getchar()) != EOF) {
        if (c == '\\' && !char_escape_mode_enabled) {
            char_escape_mode_enabled = true;
            
            continue;
        }

        if (c == '"' && !char_escape_mode_enabled) {
            quote_mode_enabled = !quote_mode_enabled;

            if (!quote_mode_enabled && word == NULL) {
                within_word = true;
                word = strdup("");
            }
            
            continue;
        }

        if (c == '\n') {
            if (quote_mode_enabled == true) {
                printf("Error: unmatched quotes\n");
            } else {
                if (word != NULL) {
                    push_word_item(&first, &last, word);
                }

                for (tmp = first; tmp; tmp = tmp->next) {
                    printf("[%s]\n", tmp->word);
                }
            }
            
            quote_mode_enabled = false;
            i = 0;
            within_word = false;
            free(word);
            word = NULL;

            free_word_item_list(first);
            first = last = NULL;

            print_input_prompt();
            
            continue;
        }

        if ((c == ' ' || c == '\t') && quote_mode_enabled == false) {
            if (within_word == true) {
                i = 0;
                push_word_item(&first, &last, word);
                free(word);
                word = NULL;
            }

            within_word = false;

            continue;
        }
        
        if ((c != '\\' && c != '"') || char_escape_mode_enabled) {
            if (within_word == false) {
                word = malloc(buffer * sizeof(char));
            }

            if (i > buffer - 1 - 1) {
                buffer *= 2;
                word = realloc(word, buffer);
            }
            word[i] = c;
            word[i + 1] = '\0';
            i++;
         
            within_word = true;
            char_escape_mode_enabled = false;
        }
    }

    if (isatty(STDIN_FILENO)) {
        putchar('\n');
    }

    return 0;
}
