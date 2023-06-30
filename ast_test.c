#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

void test_ast_pipeline_creation()
{
    struct ast *ast = ast_create_pipe(false);
    char *argv1[] = {"ls",  "-al", NULL};
    char *argv2[] = {"ps",  "axu", NULL};

    ast->left = ast_create_command(argv1, NULL, false);
    ast->right = ast_create_command(argv2, NULL, false);

    printf("%s\n", ast->left->data.command.argv[0]);
    printf("%s\n", ast->right->data.command.argv[0]);
}

int main()
{
    test_ast_pipeline_creation();

    return 0;
}
