CC = gcc
CFLAGS = -g -Wall -ansi -pedantic
SRCMODULES = dynamic_char_array.c word_item.c tokenizer.c shell.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: clean shell
	./shell

shell: main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f *.o shell
