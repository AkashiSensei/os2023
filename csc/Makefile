.PHONY: clean

fibo: ./code/main.o ./code/fibo.o
	gcc ./code/main.o ./code/fibo.o -o fibo

./code/main.o: ./code/main.c
	$(MAKE) -C ./code main.o

./code/fibo.o: ./code/fibo.c
	$(MAKE) -C ./code fibo.o

clean: 
	$(MAKE) -C ./code clean
