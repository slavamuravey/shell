#ifndef AST_H_SENTRY
#define AST_H_SENTRY

#include <stdbool.h>
#include "dynamic_array.h"

struct ast_data_script {
    struct dynamic_array *expressions;
};

struct ast_data_command {
    struct dynamic_array *words;
    struct dynamic_array *redirects;
};

struct ast_data_pipeline {
    struct dynamic_array *expressions;
};

enum ast_data_expression_redirect_type {
    AST_DATA_EXPRESSION_REDIRECT_TYPE_INPUT,
    AST_DATA_EXPRESSION_REDIRECT_TYPE_OUTPUT,
    AST_DATA_EXPRESSION_REDIRECT_TYPE_OUTPUT_APPEND
};

struct ast_data_expression_redirect {
    enum ast_data_expression_redirect_type type;
    char *file;
};

enum ast_data_logical_expression_type {
    AST_DATA_LOGICAL_EXPRESSION_TYPE_AND,
    AST_DATA_LOGICAL_EXPRESSION_TYPE_OR
};

struct ast_data_logical_expression {
    enum ast_data_logical_expression_type type;
    struct ast *left;
    struct ast *right;
};

struct ast_data_subshell {
    struct ast *script;
    struct dynamic_array *redirects;
};

union ast_data {
    struct ast_data_script script;
    struct ast_data_command command;
    struct ast_data_pipeline pipeline;
    struct ast_data_logical_expression logical_expression;
    struct ast_data_subshell subshell;
};

enum ast_type {
    AST_TYPE_SCRIPT,
    AST_TYPE_COMMAND,
    AST_TYPE_PIPELINE,
    AST_TYPE_LOGICAL_EXPRESSION,
    AST_TYPE_SUBSHELL
};

struct ast {
    enum ast_type type;
    union ast_data data;
    bool async;
};

struct ast *ast_create_script();
struct ast *ast_create_command();
struct ast *ast_create_pipeline();
struct ast *ast_create_logical_expression(enum ast_data_logical_expression_type type);
struct ast *ast_create_subshell();
struct ast_data_expression_redirect *ast_data_expression_redirect_create(enum ast_data_expression_redirect_type type, char *file);
void ast_destroy(struct ast *ast);

#endif
