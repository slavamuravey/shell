#include <stdio.h>
#include "dynamic_array.h"
#include "ast.h"
#include "ast_utils.h"

static void print_ast_script(struct ast *ast)
{
    int i;
    struct dynamic_array *expressions_array = ast->data.script.expressions;
    struct ast **expressions = expressions_array->ptr;

    printf("\"expressions\": ");
    printf("[");
    
    for (i = 0; i < expressions_array->len; i++) {
        struct ast *expression = expressions[i];
        print_ast(expression);
        if (i < expressions_array->len - 1) {
            printf(", ");
        }
    }

    printf("]");
    printf(",");
}

static void print_ast_data_expression_redirect(struct ast_data_expression_redirect *redirect)
{
    if (!redirect) {
        printf("null");

        return;
    }

    printf("{");
    printf("\"type\": %d", redirect->type);
    printf(", ");
    printf("\"file\": \"%s\"", redirect->file);
    printf("}");
}

static void print_ast_command(struct ast *ast)
{
    int i;
    struct dynamic_array *words_array = ast->data.command.words;
    struct dynamic_array *redirects_array = ast->data.command.redirects;
    char **words = words_array->ptr;
    struct ast_data_expression_redirect **redirects = redirects_array->ptr;
    
    printf("\"words\": ");
    printf("[");

    for (i = 0; i < words_array->len; i++) {
        printf("\"%s\"", words[i]);
        if (i < words_array->len - 1) {
            printf(", ");
        }
    }

    printf("]");
    printf(",");
    printf("\"redirects\": ");
    printf("[");

    for (i = 0; i < redirects_array->len; i++) {
        print_ast_data_expression_redirect(redirects[i]);
        if (i < redirects_array->len - 1) {
            printf(", ");
        }
    }

    printf("]");
    printf(",");
}

static void print_ast_pipeline(struct ast *ast)
{
    int i;
    struct dynamic_array *expressions_array = ast->data.pipeline.expressions;
    struct ast **expressions = expressions_array->ptr;

    printf("\"expressions\": ");
    printf("[");
    
    for (i = 0; i < expressions_array->len; i++) {
        struct ast *expression = expressions[i];
        print_ast(expression);
        if (i < expressions_array->len - 1) {
            printf(", ");
        }
    }

    printf("]");
    printf(",");
}

static void print_ast_logical_expression(struct ast *ast)
{
    printf("\"left\": ");
    print_ast(ast->data.logical_expression.left);
    printf(",");
    printf("\"right\": ");
    print_ast(ast->data.logical_expression.right);
    printf(",");
}

static void print_ast_subshell(struct ast *ast)
{
    int i;
    struct dynamic_array *redirects_array = ast->data.subshell.redirects;
    struct ast_data_expression_redirect **redirects = redirects_array->ptr;

    printf("\"script\": ");
    print_ast(ast->data.subshell.script);
    printf(",");
    printf("\"redirects\": ");
    printf("[");

    for (i = 0; i < redirects_array->len; i++) {
        print_ast_data_expression_redirect(redirects[i]);
        if (i < redirects_array->len - 1) {
            printf(", ");
        }
    }

    printf("]");
    printf(",");
}

void print_ast(struct ast *ast)
{
    if (!ast) {
        printf("null");

        return;
    }

    printf("{");

    printf("\"type\": %d", ast->type);
    printf(",");

    switch (ast->type) {
    case AST_TYPE_SCRIPT:
        print_ast_script(ast);
        break;
    case AST_TYPE_COMMAND:
        print_ast_command(ast);
        break;
    case AST_TYPE_PIPELINE:
        print_ast_pipeline(ast);
        break;
    case AST_TYPE_LOGICAL_EXPRESSION:
        print_ast_logical_expression(ast);
        break;
    case AST_TYPE_SUBSHELL:
        print_ast_subshell(ast);
        break;
    }

    printf("\"async\": %d", ast->async);

    printf("}");
}
