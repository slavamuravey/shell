#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "token.h"

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

static void parser_parse_expression(struct parser *p, struct ast **ast, char **error_msg)
{
    struct token *token;

    token = NULL;
    parser_match_token(p, TOKEN_TYPE_EXPRESSION_END, &token);
    if (token) {
        struct ast *expression = ast_create_expression();
        *ast = expression;

        return;
    }

    *error_msg = "EOL token expected";

    /* token = NULL;
    parser_match_token(p, TOKEN_TYPE_WORD, &token);
    if (token) {
        struct ast *expression = ast_create_expression();
        dynamic_array_append(expression->data.expression.asts, );
        *ast = expression;
    } */
}

void parser_parse(struct parser *p, const struct dynamic_array *tokens, struct parse_data **data, struct parse_error **error)
{
    struct ast *root;
    p->tokens = tokens;
    root = ast_create_script();
    /* while (p->pos < p->tokens->len) {
        struct ast *expresstion = NULL;
        char *error_msg = NULL;
        parser_parse_expression(p, &expresstion, &error_msg);

        if (error_msg) {
            *error = parse_error_create(error_msg);

            break;
        }

        dynamic_array_append(root->data.script.expressions, &expresstion);
        
    } */
    *data = parse_data_create(root);
    parser_reset(p);
}

void parser_destroy(struct parser *p)
{
    free(p);
}
