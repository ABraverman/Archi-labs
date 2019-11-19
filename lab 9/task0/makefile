
all:  t3 task2c

task0: task0.c
	gcc -m32 -g -Wall -o myELF task0.c

task2c: task2c.c
	gcc -m32 -g -Wall -o task2c task2c.c

t1: t1.o
	ld -m elf_i386 t1.o -o t1

t1.o: t1.s
	nasm -f elf t1.s -o t1.o

t3: t3.o
	ld -m elf_i386 t3.o -o flame2

t3.o: t3-2.s
	nasm -f elf t3-2.s -o t3.o

skeleton: skeleton.o
	ld -m elf_i386 skeleton.o -o skeleton

skeleton.o: skeleton.s
	nasm -f elf skeleton.s -o skeleton.o



.PHONY: clean

clean: 
	rm -f *.o task0 skeleton flame2 t3 t1 task2c
