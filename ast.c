#include <stdlib.h>
#include "ast.h"

static struct ast *ast_create(enum ast_type type, union ast_data data)
{
    struct ast *ast = malloc(sizeof(struct ast));
    ast->type = type;
    ast->data = data;

    return ast;
}

struct ast *ast_create_script(struct dynamic_array *asts)
{
    struct ast *ast;
    union ast_data data;
    data.script.asts = asts;
    ast = ast_create(AST_TYPE_SCRIPT, data);

    return ast;
}

struct ast *ast_create_command(char **argv, struct dynamic_array *redirects, bool async)
{
    struct ast *ast;
    union ast_data data;
    data.command.argv = argv;
    data.command.redirects = redirects;
    data.command.async = async;
    ast = ast_create(AST_TYPE_COMMAND, data);

    return ast;
}

struct ast *ast_create_pipeline(struct dynamic_array *asts, bool async)
{
    struct ast *ast;
    union ast_data data;
    data.pipeline.asts = asts;
    data.command.async = async;
    ast = ast_create(AST_TYPE_PIPELINE, data);

    return ast;
}

struct ast *ast_create_logical_expression(enum ast_data_logical_expression_type type, struct ast *left, struct ast *right)
{
    struct ast *ast;
    union ast_data data;
    data.logical_expression.type = type;
    data.logical_expression.left = left;
    data.logical_expression.right = right;
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

struct ast_data_command_redirect *ast_data_command_redirect_create(enum ast_data_command_redirect_type type, char *file)
{
    struct ast_data_command_redirect *redirect = malloc(sizeof(struct ast_data_command_redirect));
    redirect->type = type;
    redirect->file = file;

    return redirect;
}

void ast_destroy(struct ast *ast)
{
    free(ast);
}
