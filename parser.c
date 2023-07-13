#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "token.h"
#include "utils.h"

static struct parse_data *parse_data_create(struct ast *ast)
{
    struct parse_data *data = malloc(sizeof(struct parse_data));
    data->ast = ast;

    return data;
}

static struct parse_error *parse_error_create(char *message)
{
    struct parse_error *error = malloc(sizeof(struct parse_error));
    error->message = message;

    return error;
}

static void parser_reset(struct parser *p)
{
    p->pos = 0;
    p->tokens = NULL;
}

struct parser *parser_create()
{
    struct parser *p = malloc(sizeof(struct parser));
    p->pos = 0;
    p->tokens = NULL;

    return p;
}

static void parser_match_token(struct parser *p, enum token_type token_type, struct token **token)
{
    struct token **tokens = p->tokens->ptr;
    struct token *current_token = tokens[p->pos];

    if (p->pos >= p->tokens->len || current_token->type != token_type) {
        *token = NULL;

        return;
    }

    if (current_token->type == token_type) {
        p->pos++;
        *token = current_token;
    }
}

static void parser_parse_command(struct parser *p, struct ast **command, char **error_msg)
{
    struct token *token = NULL;
    do {
        parser_match_token(p, TOKEN_TYPE_WORD, &token);
        if (token) {
            char *word;
            if (!*command) {
                *command = ast_create_command(false);
            }

            word = dupstr(token->text);
            dynamic_array_append((*command)->data.command.words, &word);
        }
    } while (token);
}

static void parser_parse_expression(struct parser *p, struct ast **expression, char **error_msg)
{
    struct ast *expression_command = NULL;
    struct token *logical_expression_token;
    struct ast *expression_right;
    parser_parse_command(p, &expression_command, error_msg);
    if (*error_msg) {
        return;
    }

    if (expression_command) {
        *expression = expression_command;
    }

    logical_expression_token = NULL;
    parser_match_token(p, TOKEN_TYPE_AND, &logical_expression_token);
    if (!logical_expression_token) {
        parser_match_token(p, TOKEN_TYPE_OR, &logical_expression_token);
        if (!logical_expression_token) {
            return;
        }
    }

    expression_right = NULL;
    parser_parse_expression(p, &expression_right, error_msg);
    if (*error_msg) {
        return;
    }

    if (!expression_right) {
        *error_msg = "right operand of logical expression expected";
        return;
    }

    *expression = ast_create_logical_expression(logical_expression_token->type, *expression, expression_right);
}

static void parser_parse_expression_separator(struct parser *p, struct ast *expression, char **error_msg)
{
    struct token *expression_separator_token = NULL;
    if (!expression) {
        parser_match_token(p, TOKEN_TYPE_EXPRESSION_SEPARATOR_2, &expression_separator_token);
    } else {
        parser_match_token(p, TOKEN_TYPE_EXPRESSION_SEPARATOR_1, &expression_separator_token);

        if (!expression_separator_token) {
            parser_match_token(p, TOKEN_TYPE_EXPRESSION_SEPARATOR_2, &expression_separator_token);

            if (!expression_separator_token) {
                *error_msg = "expression separator token expected";

                return;
            }
        }
    }

    if (!expression_separator_token) {
        *error_msg = "unexpected token";
    }
}

void parser_parse(struct parser *p, const struct dynamic_array *tokens, struct parse_data **data, struct parse_error **error)
{
    struct ast *root;
    char *error_msg = NULL;
    p->tokens = tokens;
    root = ast_create_script();
    while (p->pos < p->tokens->len) {
        struct ast *expression = NULL;
        parser_parse_expression(p, &expression, &error_msg);
        if (error_msg) {
            break;
        }

        parser_parse_expression_separator(p, expression, &error_msg);
        if (error_msg) {
            break;
        }

        if (expression) {
            dynamic_array_append(root->data.script.expressions, &expression);
        }
    }
    if (p->pos < p->tokens->len) {
        *error = parse_error_create(error_msg);
    }
    *data = parse_data_create(root);
    parser_reset(p);
}

void parser_destroy(struct parser *p)
{
    free(p);
}
