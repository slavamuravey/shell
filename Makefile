CC = gcc
CFLAGS = -g -Wall -ansi -pedantic
SRCMODULES = utils.c dynamic_array.c token.c tokenizer.c ast.c parser.c vm.c shell.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: clean shell
	./shell

-include ast.mk
-include dynamic_array.mk
-include tokenizer.mk

shell: main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: test
test: ast_test_run dynamic_array_test_run tokenizer_test_run

.PHONY: clean
clean:
	rm -f *.o shell *_test
