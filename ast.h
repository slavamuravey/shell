#ifndef AST_SENTRY
#define AST_SENTRY

#include <stdbool.h>

struct ast_data_command {
    char **argv;
    struct ast_data_command_redirect *redirect;
    bool async;
};

struct ast_data_pipe {
    bool async;
};

enum ast_data_command_redirect_type {
    INPUT,
    OUTPUT,
    OUTPUT_APPEND
};

struct ast_data_command_redirect {
    enum ast_data_command_redirect_type type;
    char *file;
    struct ast_data_command_redirect *next;
};

enum ast_data_logical_expression_type {
    AND,
    OR
};

struct ast_data_logical_expression {
    enum ast_data_logical_expression_type type;
};

struct ast_data_subshell {
    char *cmd;
    bool async;
};

union ast_data {
    struct ast_data_command command;
    struct ast_data_logical_expression logical_expression;
    struct ast_data_subshell subshell;
};

enum ast_type {
    AST_TYPE_COMMAND,
    AST_TYPE_PIPE,
    AST_TYPE_LOGICAL_EXPRESSION,
    AST_TYPE_SUBSHELL,
    AST_TYPE_SEMICOLON
};

struct ast {
    enum ast_type type;
    union ast_data data;
    struct ast *left;
    struct ast *right;
};

struct ast *ast_create_command(char **argv, struct ast_data_command_redirect *redirect, bool async);
struct ast *ast_create_pipe(bool async);
struct ast *ast_create_logical_expression(enum ast_data_logical_expression_type type);
struct ast *ast_create_subshell(char *cmd, bool async);
struct ast *ast_create_semicolon();
void ast_destroy(struct ast *ast);

#endif
