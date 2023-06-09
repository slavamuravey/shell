#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "ast.h"

void test_ast_script_creation()
{
    struct ast *script = ast_create_script();
    struct ast *expression = ast_create_expression();
    struct ast **expression_asts;
    struct ast **script_expressions;
    struct ast *script_expression;
    struct ast *ast1;
    struct ast *ast2;
    char **words1;
    char **words2;
    char *word;

    struct ast *cmd1 = ast_create_command(false);
    struct ast *cmd2 = ast_create_command(false);

    word = dupstr("ls");
    dynamic_array_append(cmd1->data.command.words, &word);
    word = dupstr("-al");
    dynamic_array_append(cmd1->data.command.words, &word);

    word = dupstr("ps");
    dynamic_array_append(cmd2->data.command.words, &word);
    word = dupstr("axu");
    dynamic_array_append(cmd2->data.command.words, &word);

    dynamic_array_append(expression->data.expression.asts, &cmd1);
    dynamic_array_append(expression->data.expression.asts, &cmd2);

    dynamic_array_append(script->data.script.expressions, &expression);
    
    script_expressions = script->data.script.expressions->ptr;
    script_expression = script_expressions[0];
    expression_asts = script_expression->data.expression.asts->ptr;
    ast1 = expression_asts[0];
    ast2 = expression_asts[1];

    words1 = ast1->data.command.words->ptr;
    words2 = ast2->data.command.words->ptr;
    printf("%s\n", words1[0]);
    printf("%s\n", words2[0]);
    ast_destroy(script);
}

void test_ast_pipeline_creation()
{
    struct ast *pipeline = ast_create_pipeline(false);
    struct ast **pipeline_asts;
    struct ast *ast1;
    struct ast *ast2;
    char **words1;
    char **words2;
    char *word;

    struct ast *cmd1 = ast_create_command(false);
    struct ast *cmd2 = ast_create_command(false);

    word = dupstr("ls");
    dynamic_array_append(cmd1->data.command.words, &word);
    word = dupstr("-al");
    dynamic_array_append(cmd1->data.command.words, &word);

    word = dupstr("ps");
    dynamic_array_append(cmd2->data.command.words, &word);
    word = dupstr("axu");
    dynamic_array_append(cmd2->data.command.words, &word);

    dynamic_array_append(pipeline->data.pipeline.asts, &cmd1);
    dynamic_array_append(pipeline->data.pipeline.asts, &cmd2);
    
    pipeline_asts = pipeline->data.pipeline.asts->ptr;
    ast1 = pipeline_asts[0];
    ast2 = pipeline_asts[1];

    words1 = ast1->data.command.words->ptr;
    words2 = ast2->data.command.words->ptr;
    printf("%s\n", words1[0]);
    printf("%s\n", words2[0]);
    ast_destroy(pipeline);
}

void test_ast_command_creation()
{
    char *word;
    struct ast_data_command_redirect **command_redirects;
    struct ast_data_command_redirect *command_redirect;
    struct ast_data_command_redirect *redirect;

    struct ast *command = ast_create_command(false);

    word = dupstr("ls");
    dynamic_array_append(command->data.command.words, &word);
    word = dupstr("-al");
    dynamic_array_append(command->data.command.words, &word);

    redirect = ast_data_command_redirect_create(AST_DATA_COMMAND_REDIRECT_TYPE_INPUT, dupstr("file.txt"));

    dynamic_array_append(command->data.command.redirects, &redirect);

    command_redirects = command->data.command.redirects->ptr;
    command_redirect = command_redirects[0];

    printf("%d %s\n", command_redirect->type, command_redirect->file);
    ast_destroy(command);
}

int main()
{
    test_ast_script_creation();
    printf("\n");
    test_ast_pipeline_creation();
    printf("\n");
    test_ast_command_creation();

    return 0;
}
