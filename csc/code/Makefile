.PHONY: clean all

all: main.o fibo.o

main.o: main.c
	gcc -c main.c -o main.o -I../include

fibo.o: fibo.c
	gcc -c fibo.c -o fibo.o -I../include

clean: 
	rm -f main.o fibo.o
