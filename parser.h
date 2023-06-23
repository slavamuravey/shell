#ifndef PARSER_SENTRY
#define PARSER_SENTRY

#include "token.h"
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
void parser_parse(struct parser *p, struct token *token_first, struct parse_data **data, struct parse_error **error);
void parser_destroy(struct parser *p);

#endif