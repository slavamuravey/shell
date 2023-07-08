#ifndef AST_SENTRY
#define AST_SENTRY

#include <stdbool.h>
#include "dynamic_array.h"

struct ast_data_script {
    struct dynamic_array *asts;
};

struct ast_data_command {
    struct dynamic_array *words;
    struct dynamic_array *redirects;
    bool async;
};

struct ast_data_pipeline {
    bool async;
    struct dynamic_array *asts;
};

enum ast_data_command_redirect_type {
    AST_DATA_COMMAND_REDIRECT_TYPE_INPUT,
    AST_DATA_COMMAND_REDIRECT_TYPE_OUTPUT,
    AST_DATA_COMMAND_REDIRECT_TYPE_OUTPUT_APPEND
};

struct ast_data_command_redirect {
    enum ast_data_command_redirect_type type;
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
    bool async;
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
};

struct ast *ast_create_script();
struct ast *ast_create_command(bool async);
struct ast *ast_create_pipeline(bool async);
struct ast *ast_create_logical_expression(enum ast_data_logical_expression_type type, struct ast *left, struct ast *right);
struct ast *ast_create_subshell(bool async);
struct ast_data_command_redirect *ast_data_command_redirect_create(enum ast_data_command_redirect_type type, char *file);
void ast_destroy(struct ast *ast);

#endif
