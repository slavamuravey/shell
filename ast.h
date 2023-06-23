#ifndef AST_SENTRY
#define AST_SENTRY

struct ast {
};

struct ast *ast_create();
void ast_destroy(struct ast *ast);

#endif
