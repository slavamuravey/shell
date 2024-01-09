#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "token.h"
#include "utils.h"
#include "dynamic_array.h"
#include "ast.h"

static void parser_parse_expression(struct parser *p, struct ast **expression, char **error_msg);
static void parser_parse_expression_separator(struct parser *p, struct ast **expression, char **error_msg);

static struct parse_data *parse_data_create(struct ast *ast)
{
    struct parse_data *data = malloc(sizeof(struct parse_data));
    data->ast = ast;

    return data;
}

static struct parse_error *parse_error_create(char *message)
{
    struct parse_error *error = malloc(sizeof(struct parse_error));
    error->message = message;

    return error;
}

static void parser_reset(struct parser *p)
{
    p->pos = 0;
    p->tokens = NULL;
}

struct parser *parser_create()
{
    struct parser *p = malloc(sizeof(struct parser));
    p->pos = 0;
    p->tokens = NULL;

    return p;
}

static void parser_match_token(struct parser *p, enum token_type token_type, struct token **token)
{
    struct token **tokens = p->tokens->ptr;
    struct token *current_token = tokens[p->pos];

    if (p->pos >= p->tokens->len || current_token->type != token_type) {
        *token = NULL;

        return;
    }

    if (current_token->type == token_type) {
        p->pos++;
        *token = current_token;
    }
}

static void parser_append_expression_redirect(struct parser *p, struct ast **expression, struct ast_data_expression_redirect *redirect)
{
    switch ((*expression)->type) {
    case AST_TYPE_COMMAND: 
        dynamic_array_append((*expression)->data.command.redirects, &redirect);
        break;
    case AST_TYPE_SUBSHELL: 
        dynamic_array_append((*expression)->data.subshell.redirects, &redirect);
        break;
    default:
        break;
    }
}

static void parser_parse_redirects_input(struct parser *p, struct ast **expression, char **error_msg)
{
    struct token *redirect_input_token = NULL;
    struct token *word_token = NULL;
    struct ast_data_expression_redirect *redirect;

    parser_match_token(p, TOKEN_TYPE_REDIRECT_INPUT, &redirect_input_token);
    if (!redirect_input_token) {
        return;
    }

    parser_match_token(p, TOKEN_TYPE_WORD, &word_token);
    if (!word_token) {
        *error_msg = "unexpected redirect operand";

        return;
    }

    redirect = ast_data_expression_redirect_create(AST_DATA_EXPRESSION_REDIRECT_TYPE_INPUT, dupstr(word_token->text));
    parser_append_expression_redirect(p, expression, redirect);
}

static void parser_parse_redirects_output(struct parser *p, struct ast **expression, char **error_msg)
{
    struct token *redirect_output_token = NULL;
    struct token *word_token = NULL;
    struct ast_data_expression_redirect *redirect;
    enum ast_data_expression_redirect_type redirect_type = AST_DATA_EXPRESSION_REDIRECT_TYPE_OUTPUT;

    parser_match_token(p, TOKEN_TYPE_REDIRECT_OUTPUT, &redirect_output_token);
    if (!redirect_output_token) {
        parser_match_token(p, TOKEN_TYPE_REDIRECT_OUTPUT_APPEND, &redirect_output_token);
        if (!redirect_output_token) {
            return;
        }
        redirect_type = AST_DATA_EXPRESSION_REDIRECT_TYPE_OUTPUT_APPEND;
    }

    parser_match_token(p, TOKEN_TYPE_WORD, &word_token);
    if (!word_token) {
        *error_msg = "unexpected redirect operand";

        return;
    }

    redirect = ast_data_expression_redirect_create(redirect_type, dupstr(word_token->text));
    parser_append_expression_redirect(p, expression, redirect);
}

static void parser_parse_redirects(struct parser *p, struct ast **expression, char **error_msg)
{
    int pos = p->pos;
    parser_parse_redirects_input(p, expression, error_msg);
    if (p->pos != pos) {
        parser_parse_redirects_output(p, expression, error_msg);

        return;
    }
    
    parser_parse_redirects_output(p, expression, error_msg);
    parser_parse_redirects_input(p, expression, error_msg);
}

static void parser_parse_command(struct parser *p, struct ast **command, char **error_msg)
{
    struct token *word_token = NULL;
    char *word;
    
    while (true) {
        parser_match_token(p, TOKEN_TYPE_WORD, &word_token);
        if (!word_token) {
            break;
        }

        if (!*command) {
            *command = ast_create_command();
        }

        word = dupstr(word_token->text);
        dynamic_array_append((*command)->data.command.words, &word);
    };
}

static void parser_parse_subshell(struct parser *p, struct ast **subshell, char **error_msg)
{
    struct token *subshell_start_token;
    struct token *subshell_end_token;
    struct dynamic_array *expressions_array;
    struct ast *script;

    subshell_start_token = NULL;
    subshell_end_token = NULL;

    parser_match_token(p, TOKEN_TYPE_SUBSHELL_START, &subshell_start_token);
    if (!subshell_start_token) {
        parser_parse_command(p, subshell, error_msg);

        return;
    }

    *subshell = ast_create_subshell();
    script = ast_create_script();
    (*subshell)->data.subshell.script = script;
    expressions_array = script->data.script.expressions;

    while (true) {
        struct ast *expression = NULL;
        parser_parse_expression(p, &expression, error_msg);
        dynamic_array_append(expressions_array, &expression);
        if (*error_msg) {
            return;
        }

        parser_match_token(p, TOKEN_TYPE_SUBSHELL_END, &subshell_end_token);
        if (subshell_end_token) {
            if (!expression && expressions_array->len == 1) {
                *error_msg = "unexpected token";

                return;
            }
            
            break;
        }

        parser_parse_expression_separator(p, &expression, error_msg);
        if (*error_msg) {
            return;
        }
    }
}

static void parser_parse_pipeline(struct parser *p, struct ast **expression, char **error_msg)
{
    struct token *pipe_token;
    struct ast *pipeline = NULL;

    while (true) {
        struct ast *subshell = NULL;
        parser_parse_subshell(p, &subshell, error_msg);
        if (*error_msg) {
            *expression = subshell;
            
            return;
        }

        parser_parse_redirects(p, &subshell, error_msg);

        pipe_token = NULL;
        parser_match_token(p, TOKEN_TYPE_PIPE, &pipe_token);
        if (!pipe_token) {
            if (!pipeline) {
                *expression = subshell;
            } else {
                subshell->async = true;
                dynamic_array_append(pipeline->data.pipeline.expressions, &subshell);

                if (!subshell) {
                    *error_msg = "unexpected pipeline right operand";
                }
            }

            break;
        }

        if (!pipeline) {
            pipeline = ast_create_pipeline();
            *expression = pipeline;
        }
        
        subshell->async = true;
        dynamic_array_append(pipeline->data.pipeline.expressions, &subshell);

        if (!subshell) {
            *error_msg = "unexpected pipeline left operand";

            return;
        }
    }
}

static void parser_parse_expression(struct parser *p, struct ast **expression, char **error_msg)
{
    struct token *logical_expression_token;
    enum ast_data_logical_expression_type type;
    struct ast *tmp_expression;
    struct ast *pipeline = NULL;
    
    parser_parse_pipeline(p, &pipeline, error_msg);
    if (*error_msg) {
        return;
    }

    logical_expression_token = NULL;
    parser_match_token(p, TOKEN_TYPE_AND, &logical_expression_token);
    if (!logical_expression_token) {
        parser_match_token(p, TOKEN_TYPE_OR, &logical_expression_token);
        if (!logical_expression_token) {
            if (!*expression) {
                *expression = pipeline;

                return;
            }
            
            if (!pipeline) {
                *error_msg = "unexpected logical expression operand";

                return;
            }
            
            (*expression)->data.logical_expression.right = pipeline;
            
            return;
        }
    }

    if (!pipeline) {
        *error_msg = "unexpected logical expression operand";

        return;
    }

    type = logical_expression_token->type == TOKEN_TYPE_AND ? 
        AST_DATA_LOGICAL_EXPRESSION_TYPE_AND : 
        AST_DATA_LOGICAL_EXPRESSION_TYPE_OR;
    tmp_expression = ast_create_logical_expression(type);
    
    if (*expression) {
        tmp_expression->data.logical_expression.left = *expression;
        (*expression)->data.logical_expression.right = pipeline;
    } else {
        tmp_expression->data.logical_expression.left = pipeline;
    }
    
    *expression = tmp_expression;
    parser_parse_expression(p, expression, error_msg);
}

static void parser_parse_expression_separator(struct parser *p, struct ast **expression, char **error_msg)
{
    struct token *expression_separator_token = NULL;
    if (!*expression) {
        parser_match_token(p, TOKEN_TYPE_EXPRESSION_SEPARATOR_2, &expression_separator_token);
        if (!expression_separator_token) {
            *error_msg = "unexpected token";
        }
    } else {
        parser_match_token(p, TOKEN_TYPE_EXPRESSION_SEPARATOR_1, &expression_separator_token);
        if (expression_separator_token) {
            return;
        }

        parser_match_token(p, TOKEN_TYPE_ASYNC, &expression_separator_token);
        if (expression_separator_token) {
            (*expression)->async = true;
            
            return;
        }

        parser_match_token(p, TOKEN_TYPE_EXPRESSION_SEPARATOR_2, &expression_separator_token);
        if (!expression_separator_token) {
            *error_msg = "expression separator token expected";
        }
    }    
}

void parser_parse(struct parser *p, const struct dynamic_array *tokens, struct parse_data **data, struct parse_error **error)
{
    struct ast *root;
    char *error_msg = NULL;
    p->tokens = tokens;
    root = ast_create_script();
    while (p->pos < p->tokens->len) {
        struct ast *expression = NULL;
        parser_parse_expression(p, &expression, &error_msg);
        dynamic_array_append(root->data.script.expressions, &expression);
        if (error_msg) {
            break;
        }

        parser_parse_expression_separator(p, &expression, &error_msg);
        if (error_msg) {
            break;
        }
    }
    if (error_msg) {
        *error = parse_error_create(error_msg);
    }
    *data = parse_data_create(root);
    parser_reset(p);
}

void parser_destroy(struct parser *p)
{
    free(p);
}
