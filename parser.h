#ifndef PARSER_SENTRY
#define PARSER_SENTRY

#include "dynamic_array.h"
#include "ast.h"

struct parser {
};

struct parse_data {
    struct ast *ast;
};

struct parse_error {
    char *message;
};

struct parser *parser_create();
void parser_parse(struct parser *p, struct dynamic_array *tokens, struct parse_data **data, struct parse_error **error);
void parser_destroy(struct parser *p);

#endif