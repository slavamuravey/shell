#include <stdlib.h>
#include "ast.h"

struct ast *ast_create()
{
    struct ast *ast = malloc(sizeof(struct ast));

    return ast;
}

void ast_destroy(struct ast *ast)
{
    free(ast);
}
