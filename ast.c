#include <stdlib.h>
#include "ast.h"

static struct ast *ast_create(enum ast_type type, union ast_data data)
{
    struct ast *ast = malloc(sizeof(struct ast));
    ast->type = type;
    ast->data = data;
    ast->left = NULL;
    ast->right = NULL;

    return ast;
}

struct ast *ast_create_command(char **argv, struct ast_data_command_redirect *redirect, bool async)
{
    struct ast *ast;
    union ast_data data;
    data.command.argv = argv;
    data.command.redirect = redirect;
    data.command.async = async;
    ast = ast_create(AST_TYPE_COMMAND, data);

    return ast;
}

struct ast *ast_create_pipe(bool async)
{
    struct ast *ast;
    union ast_data data;
    data.command.async = async;
    ast = ast_create(AST_TYPE_PIPE, data);

    return ast;
}

struct ast *ast_create_logical_expression(enum ast_data_logical_expression_type type)
{
    struct ast *ast;
    union ast_data data;
    data.logical_expression.type = type;
    ast = ast_create(AST_TYPE_LOGICAL_EXPRESSION, data);

    return ast;
}

struct ast *ast_create_subshell(char *cmd, bool async)
{
    struct ast *ast;
    union ast_data data;
    data.subshell.cmd = cmd;
    data.command.async = async;
    ast = ast_create(AST_TYPE_SUBSHELL, data);

    return ast;
}

struct ast *ast_create_semicolon()
{
    union ast_data data;
    struct ast *ast = ast_create(AST_TYPE_SEMICOLON, data);

    return ast;
}

void ast_destroy(struct ast *ast)
{
    free(ast);
}
