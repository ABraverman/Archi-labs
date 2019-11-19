
all: Main

Main: main.o add.o numbers.o
	gcc -m32 -g -Wall -o Main main.o add.o numbers.o

main.o: main.c
	gcc -m32 -g -Wall -c -o main.o main.c

add.o: add.s
	nasm -g -f elf -w+all -o add.o add.s
	
numbers.o: numbers.c
	gcc -m32 -g -Wall -c -o numbers.o numbers.c



	
.PHONY: clean

clean: 
	rm -f *.o Main
