#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

void test_ast_script_creation()
{
    struct dynamic_array *asts = dynamic_array_create(1, sizeof(struct ast));
    struct ast *script = ast_create_script(asts);
    struct ast *script_asts;
    struct ast ast1;
    struct ast ast2;

    char *argv1[] = {"ls",  "-al", NULL};
    char *argv2[] = {"ps",  "axu", NULL};

    dynamic_array_append(script->data.script.asts, ast_create_command(argv1, NULL, false));
    dynamic_array_append(script->data.script.asts, ast_create_command(argv2, NULL, false));
    
    script_asts = script->data.script.asts->ptr;
    ast1 = script_asts[0];
    ast2 = script_asts[1];

    printf("%s\n", ast1.data.command.argv[0]);
    printf("%s\n", ast2.data.command.argv[0]);
}

void test_ast_pipeline_creation()
{
    struct dynamic_array *asts = dynamic_array_create(1, sizeof(struct ast));
    struct ast *pipeline = ast_create_pipeline(asts, false);
    struct ast *pipeline_asts;
    struct ast ast1;
    struct ast ast2;

    char *argv1[] = {"ls",  "-al", NULL};
    char *argv2[] = {"ps",  "axu", NULL};

    dynamic_array_append(pipeline->data.pipeline.asts, ast_create_command(argv1, NULL, false));
    dynamic_array_append(pipeline->data.pipeline.asts, ast_create_command(argv2, NULL, false));
    
    pipeline_asts = pipeline->data.pipeline.asts->ptr;
    ast1 = pipeline_asts[0];
    ast2 = pipeline_asts[1];

    printf("%s\n", ast1.data.command.argv[0]);
    printf("%s\n", ast2.data.command.argv[0]);
}

void test_ast_command_creation()
{
    struct dynamic_array *redirects = dynamic_array_create(1, sizeof(struct ast_data_command_redirect));
    char *argv[] = {"ls",  "-al", NULL};
    struct ast *command = ast_create_command(argv, redirects, false);
    struct ast_data_command_redirect *command_redirects;
    struct ast_data_command_redirect command_redirect;

    dynamic_array_append(command->data.command.redirects, ast_data_command_redirect_create(AST_DATA_COMMAND_REDIRECT_TYPE_INPUT, "file.txt"));

    command_redirects = command->data.command.redirects->ptr;
    command_redirect = command_redirects[0];

    printf("%d %s\n", command_redirect.type, command_redirect.file);
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
