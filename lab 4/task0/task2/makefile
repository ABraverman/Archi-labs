
all: Main

Main: main.o start.o util.o
	ld -g -m elf_i386 start.o main.o util.o -o flame2

start.o: start.s
	nasm -g -f elf start.s -o start.o

util.o: util.c
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

	
main.o: main.c
	gcc -g -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o



	
.PHONY: clean

clean: 
	rm -f *.o encoder
