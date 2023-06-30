.PHONY: ast_test_run
ast_test_run: clean ast_test
	./ast_test

ast_test: ast_test.c ast.o
	$(CC) $(CFLAGS) $^ -o $@
