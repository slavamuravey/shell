.PHONY: ast_test_run
ast_test_run: clean ast_test
	@echo "+ $@" 
	./ast_test

ast_test: ast_test.c dynamic_array.o ast.o
	$(CC) $(CFLAGS) $^ -o $@
