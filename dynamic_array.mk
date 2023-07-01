.PHONY: dynamic_array_test_run
dynamic_array_test_run: clean dynamic_array_test
	./dynamic_array_test

dynamic_array_test: dynamic_array_test.c dynamic_array.o
	$(CC) $(CFLAGS) $^ -o $@
