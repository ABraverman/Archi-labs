
all: task1 ntsc_fix

task1: task1.c
	gcc -m32 -g -Wall -o hexeditplus task1.c

task4: task4.c
	gcc -m32 -g -Wall -fno-pic -o task4 task4.c

.PHONY: clean

clean: 
	rm -f hexeditplus task4
