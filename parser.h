#ifndef PARSER_H_SENTRY
#define PARSER_H_SENTRY

#include "dynamic_array.h"
#include "ast.h"

struct parser {
    size_t pos;
    const struct dynamic_array *tokens;
};

struct parse_data {
    struct ast *ast;
};

struct parse_error {
    char *message;
};

struct parser *parser_create();
void parser_parse(struct parser *p, const struct dynamic_array *tokens, struct parse_data **data, struct parse_error **error);
void parser_destroy(struct parser *p);

#endif
