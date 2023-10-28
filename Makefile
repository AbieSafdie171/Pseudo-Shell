all: pseudo-shell.exe

pseudo-shell.exe: main.o command.o string_parser.o
	gcc -o pseudo-shell.exe main.o command.o string_parser.o

main.o: main.c
	gcc -c main.c

command.o: command.c command.h
	gcc -c command.c

string_parser.o: string_parser.c string_parser.h
	gcc -c string_parser.c

clean:
	rm -f core *.o pseudo-shell.exe
