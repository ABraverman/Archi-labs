
all: Main

Main: main.o start.o util.o
	ld -m elf_i386 start.o main.o util.o -o encoder

start.o: start.s
	nasm -f elf start.s -o start.o

util.o: util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

	
main.o: task1.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector task1.c -o main.o



	
.PHONY: clean

clean: 
	rm -f *.o encoder
