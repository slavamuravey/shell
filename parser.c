#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

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

void parser_parse(struct parser *p, const struct dynamic_array *tokens, struct parse_data **data, struct parse_error **error)
{
    struct ast *ast;
    char *argv[] = {NULL};
    p->tokens = tokens;
    ast = ast_create_command(argv, NULL, false);
    *data = parse_data_create(ast);
    parser_reset(p);
}

void parser_destroy(struct parser *p)
{
    free(p);
}
