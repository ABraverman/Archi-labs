
all: task1 ntsc_fix

task1: task1.c
	gcc -m32 -g -Wall -o hexeditplus task1.c

ntsc_fix: ntsc_fix.c
	gcc -m32 -g -Wall -fno-pic -o ntsc_fix ntsc_fix.c

.PHONY: clean

clean: 
	rm -f hexeditplus ntsc_fix
