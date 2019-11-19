
all: myshell looper

myshell: task0.o LineParser.o
	gcc -m32 -g -Wall -o myshell task0.o LineParser.o

task0.o: task0.c
	gcc -m32 -g -Wall -c -o task0.o task0.c

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c

looper: looper.o
	gcc -m32 -g -Wall -o looper looper.o

looper.o: looper.c
	gcc -m32 -g -Wall -c -o looper.o looper.c

	
.PHONY: clean

clean: 
	rm -f *.o myshell looper
