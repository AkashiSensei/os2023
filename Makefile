.PHONY: run clean

test: test.c
	gcc test.c -o test

run: test
	./test

clean: 
	rm -f test
