
all: myshell looper mypipe mypipeline

myshell: task2.o LineParser.o
	gcc -m32 -g -Wall -o myshell task2.o LineParser.o

task2.o: task2.c
	gcc -m32 -g -Wall -c -o task2.o task2.c

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c

looper: looper.o
	gcc -m32 -g -Wall -o looper looper.o

looper.o: looper.c
	gcc -m32 -g -Wall -c -o looper.o looper.c

mypipe: mypipe.o
	gcc -m32 -g -Wall -o mypipe mypipe.o

mypipe.o: mypipe.c
	gcc -m32 -g -Wall -c -o mypipe.o mypipe.c

mypipeline: mypipeline.o
	gcc -m32 -g -Wall -o mypipeline mypipeline.o

mypipeline.o: mypipeline.c
	gcc -m32 -g -Wall -c -o mypipeline.o mypipeline.c

	
.PHONY: clean

clean: 
	rm -f *.o myshell looper mypipe mypipeline
