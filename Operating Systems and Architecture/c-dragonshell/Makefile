dragonshell: dragonshell.o
	gcc -Wall -o dragonshell dragonshell.o

compile:
	gcc -c dragonshell.c

clean:
	rm dragonshell *.o

run:
	gcc -Wall -o dragonshell dragonshell.c
	./dragonshell

valgrind:
	gcc -Wall -o dragonshell dragonshell.c
	valgrind --tool=memcheck --leak-check=yes -s ./dragonshell
