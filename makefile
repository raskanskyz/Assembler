all: assembler

assembler: main.o parser.o compiler.o transitions.o file_generator.o errors.o
	gcc -o assembler parser.o main.o compiler.o transitions.o file_generator.o errors.o

main.o: src/main.c
	gcc -Wall -std=c99 -pedantic -c src/main.c -I src/include

compiler.o: src/compiler.c 
	gcc -Wall -std=c99 -pedantic -c src/compiler.c -I src/include

parser.o: src/parser.c
	gcc -Wall -std=c99 -pedantic -c src/parser.c -I src/include

transitions.o: src/transitions.c
	gcc -Wall -std=c99 -pedantic -c src/transitions.c -I src/include
	
file_generator.o: src/file_generator.c
	gcc -Wall -std=c99 -pedantic -c src/file_generator.c -I src/include
	
errors.o: src/errors.c
	gcc -Wall -std=c99 -pedantic -c src/errors.c -I src/include
	
#clean 
clean:
	rm *.o assembler
