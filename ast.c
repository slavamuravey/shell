#include <stdlib.h>
#include "ast.h"

static struct ast *ast_create(enum ast_type type, union ast_data data)
{
    struct ast *ast = malloc(sizeof(struct ast));
    ast->type = type;
    ast->data = data;
    ast->async = false;

    return ast;
}

struct ast *ast_create_script()
{
    struct ast *ast;
    union ast_data data;
    data.script.expressions = dynamic_array_create(4, sizeof(struct ast*));
    ast = ast_create(AST_TYPE_SCRIPT, data);

    return ast;
}

struct ast *ast_create_command()
{
    struct ast *ast;
    union ast_data data;
    data.command.words = dynamic_array_create(4, sizeof(char *));
    data.command.redirects = dynamic_array_create(4, sizeof(struct ast_data_expression_redirect*));
    ast = ast_create(AST_TYPE_COMMAND, data);

    return ast;
}

struct ast *ast_create_pipeline()
{
    struct ast *ast;
    union ast_data data;
    data.pipeline.expressions = dynamic_array_create(4, sizeof(struct ast*));
    ast = ast_create(AST_TYPE_PIPELINE, data);

    return ast;
}

struct ast *ast_create_logical_expression(enum ast_data_logical_expression_type type)
{
    struct ast *ast;
    union ast_data data;
    data.logical_expression.type = type;
    data.logical_expression.left = NULL;
    data.logical_expression.right = NULL;
    ast = ast_create(AST_TYPE_LOGICAL_EXPRESSION, data);

    return ast;
}

struct ast *ast_create_subshell()
{
    struct ast *ast;
    union ast_data data;
    data.subshell.script = NULL;
    data.subshell.redirects = dynamic_array_create(4, sizeof(struct ast_data_expression_redirect*));
    ast = ast_create(AST_TYPE_SUBSHELL, data);

    return ast;
}

struct ast_data_expression_redirect *ast_data_expression_redirect_create(enum ast_data_expression_redirect_type type, char *file)
{
    struct ast_data_expression_redirect *redirect = malloc(sizeof(struct ast_data_expression_redirect));
    redirect->type = type;
    redirect->file = file;

    return redirect;
}

static void ast_destroy_script(struct ast *ast)
{
    struct dynamic_array *expressions_array = ast->data.script.expressions;
    int i;
    struct ast **expressions = expressions_array->ptr;
    for (i = 0; i < expressions_array->len; i++) {
        ast_destroy(expressions[i]);
    }
    free(expressions);
    free(expressions_array);
}

static void ast_data_expression_redirect_destroy(struct ast_data_expression_redirect *redirect)
{
    free(redirect->file);
    free(redirect);
}

static void ast_destroy_command(struct ast *ast)
{
    int i;
    struct dynamic_array *words_array = ast->data.command.words;
    struct dynamic_array *redirects_array = ast->data.command.redirects;
    char **words = words_array->ptr;
    struct ast_data_expression_redirect **redirects = redirects_array->ptr;
    for (i = 0; i < words_array->len; i++) {
        free(words[i]);
    }
    for (i = 0; i < redirects_array->len; i++) {
        ast_data_expression_redirect_destroy(redirects[i]);
    }
    free(words);
    free(words_array);
    free(redirects);
    free(redirects_array);
}

static void ast_destroy_pipeline(struct ast *ast)
{
    struct dynamic_array *expressions_array = ast->data.pipeline.expressions;
    int i;
    struct ast **expressions = expressions_array->ptr;
    for (i = 0; i < expressions_array->len; i++) {
        ast_destroy(expressions[i]);
    }
    free(expressions);
    free(expressions_array);
}

static void ast_destroy_logical_expression(struct ast *ast)
{
    ast_destroy(ast->data.logical_expression.left);
    ast_destroy(ast->data.logical_expression.right);
}

static void ast_destroy_subshell(struct ast *ast)
{
    int i;
    struct dynamic_array *redirects_array = ast->data.subshell.redirects;
    struct ast_data_expression_redirect **redirects = redirects_array->ptr;
    for (i = 0; i < redirects_array->len; i++) {
        ast_data_expression_redirect_destroy(redirects[i]);
    }
    free(redirects);
    free(redirects_array);
    ast_destroy(ast->data.subshell.script);
}

void ast_destroy(struct ast *ast)
{
    if (!ast) {
        return;
    }

    switch (ast->type) {
        case AST_TYPE_SCRIPT:
            ast_destroy_script(ast);
            break;
        case AST_TYPE_COMMAND:
            ast_destroy_command(ast);
            break;
        case AST_TYPE_PIPELINE:
            ast_destroy_pipeline(ast);
            break;
        case AST_TYPE_LOGICAL_EXPRESSION:
            ast_destroy_logical_expression(ast);
            break;
        case AST_TYPE_SUBSHELL:
            ast_destroy_subshell(ast);
            break;
    }

    free(ast);
}
