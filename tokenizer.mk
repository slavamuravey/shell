.PHONY: tokenizer_test_run
tokenizer_test_run: clean tokenizer_test
	@echo "+ $@"
	./tokenizer_test

tokenizer_test: tokenizer_test.c utils.o dynamic_array.o tokenizer.o token.o
	$(CC) $(CFLAGS) $^ -o $@
