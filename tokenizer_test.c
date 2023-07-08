#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"
#include "token.h"

static void run_tokenizer_tokenize_testcase(const char *str, struct tokenize_data *expected_data, struct tokenize_error *expected_error)
{
    int i;
    struct tokenize_data *t_data = NULL;
    struct tokenize_error *t_error = NULL;
    struct dynamic_array *tokens;
    struct token **actual_tokens;
    struct token **expected_tokens;
    struct tokenizer *t;

    printf("Tokenize command: %s\n", str);

    t = tokenizer_create();
    tokenizer_tokenize(t, str, &t_data, &t_error);
    tokenizer_destroy(t);

    if (t_error) {
        char *actual_message = t_error->message;
        tokens_destroy(t_data->tokens);
        free(t_data);
        free(t_error);

        if (expected_error) {
            char *expected_message = expected_error->message;
            if (strcmp(actual_message, expected_message)) {
                printf("errors missmatch: expected \"%s\", actual \"%s\"\n", expected_message, actual_message);
            }
        }

        if (expected_error == NULL) {
            printf("error is not expected, but get error with message: %s\n", actual_message);
        }

        return;
    }

    if (expected_error) {
        printf("no errors, but expected error with message: \"%s\"\n", expected_error->message);
    }
    
    tokens = t_data->tokens;
    free(t_data);
    free(t_error);

    actual_tokens = tokens->ptr;
    expected_tokens = expected_data->tokens->ptr;

    if (tokens->len != expected_data->tokens->len) {
        printf("tokens array len missmatch: expected \"%ld\", actual \"%ld\"\n", expected_data->tokens->len, tokens->len);

        tokens_destroy(tokens);

        return;
    }
    
    for (i = 0; i < tokens->len; i++) {
        printf("{type: %d, text: \"%s\"} ", actual_tokens[i]->type, actual_tokens[i]->text);
        if (actual_tokens[i]->type != expected_tokens[i]->type) {
            printf("expected type: %d, actual type: %d", expected_tokens[i]->type, actual_tokens[i]->type);
        }
        if (strcmp(actual_tokens[i]->text, expected_tokens[i]->text)) {
            printf("expected text: %s, actual text: %s", expected_tokens[i]->text, actual_tokens[i]->text);
        }
        printf("\n");
    }

    tokens_destroy(tokens);
}

static struct dynamic_array *tokens_dynamic_array_create(struct token **tokens_array, size_t tokens_array_len)
{
    struct dynamic_array *tokens;
    int i;
    tokens = dynamic_array_create(tokens_array_len, sizeof(tokens_array));
    for (i = 0; i < tokens_array_len; i++) {
        dynamic_array_append(tokens, &tokens_array[i]);
    }

    return tokens;
}

void test_tokenizer_tokenize()
{
    struct tokenize_data expected_data;
    struct tokenize_error expected_error;
    struct token *tokens_array1[24];
    struct token *tokens_array2[2];
    
    tokens_array1[0] = token_create(TOKEN_TYPE_WORD, "ls");
    tokens_array1[1] = token_create(TOKEN_TYPE_WORD, "-al");
    tokens_array1[2] = token_create(TOKEN_TYPE_PIPE, "|");
    tokens_array1[3] = token_create(TOKEN_TYPE_WORD, "cat");
    tokens_array1[4] = token_create(TOKEN_TYPE_REDIRECT_OUTPUT, ">");
    tokens_array1[5] = token_create(TOKEN_TYPE_WORD, "file.txt");
    tokens_array1[6] = token_create(TOKEN_TYPE_COMMAND_SEPARATOR, ";");
    tokens_array1[7] = token_create(TOKEN_TYPE_SUBSHELL_START, "(");
    tokens_array1[8] = token_create(TOKEN_TYPE_WORD, "true");
    tokens_array1[9] = token_create(TOKEN_TYPE_AND, "&&");
    tokens_array1[10] = token_create(TOKEN_TYPE_WORD, "false");
    tokens_array1[11] = token_create(TOKEN_TYPE_OR, "||");
    tokens_array1[12] = token_create(TOKEN_TYPE_WORD, "ps");
    tokens_array1[13] = token_create(TOKEN_TYPE_WORD, "axu");
    tokens_array1[14] = token_create(TOKEN_TYPE_REDIRECT_OUTPUT_APPEND, ">>");
    tokens_array1[15] = token_create(TOKEN_TYPE_WORD, "file2.txt");
    tokens_array1[16] = token_create(TOKEN_TYPE_REDIRECT_INPUT, "<");
    tokens_array1[17] = token_create(TOKEN_TYPE_WORD, "input.txt");
    tokens_array1[18] = token_create(TOKEN_TYPE_SUBSHELL_END, ")");
    tokens_array1[19] = token_create(TOKEN_TYPE_REDIRECT_OUTPUT, ">");
    tokens_array1[20] = token_create(TOKEN_TYPE_WORD, "file3.txt");
    tokens_array1[21] = token_create(TOKEN_TYPE_ASYNC, "&");
    tokens_array1[22] = token_create(TOKEN_TYPE_WORD, "echo");
    tokens_array1[23] = token_create(TOKEN_TYPE_WORD, "hello");

    tokens_array2[0] = token_create(TOKEN_TYPE_WORD, "ps");
    tokens_array2[1] = token_create(TOKEN_TYPE_WORD, "axu");
    
    expected_data.tokens = tokens_dynamic_array_create(tokens_array1, sizeof(tokens_array1) / sizeof(*tokens_array1));
    run_tokenizer_tokenize_testcase(" ls -al | cat > fil\"e.t\"xt;   "
    "(true && false || ps axu >> file2.txt < input.txt) > file3.txt& echo \"hello\"\n", &expected_data, NULL);
    free(expected_data.tokens);
    expected_data.tokens = NULL;

    expected_data.tokens = tokens_dynamic_array_create(tokens_array2, sizeof(tokens_array2) / sizeof(*tokens_array2));
    run_tokenizer_tokenize_testcase(" ps axu\n", &expected_data, NULL);
    free(expected_data.tokens);
    expected_data.tokens = NULL;

    expected_error.message = "unmatched quotes";
    run_tokenizer_tokenize_testcase("\"ps ", NULL, &expected_error);
}

int main()
{
    test_tokenizer_tokenize();

    return 0;
}
