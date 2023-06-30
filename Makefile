CC = gcc
CFLAGS = -g -Wall -ansi -pedantic
SRCMODULES = utils.c dynamic_char_array.c token.c tokenizer.c ast.c parser.c shell.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: clean shell
	./shell

-include ast.mk

shell: main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: test
test: ast_test_run

.PHONY: clean
clean:
	rm -f *.o shell *_test
