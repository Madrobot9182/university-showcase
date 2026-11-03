fs: fs-sim.o main.o
	gcc -Wall -g -o fs fs-sim.o main.o

compile:
	gcc -Wall -c fs-sim.c main.c

clean:
	rm -f fs *.o

run: fs
	./create_fs disk0
	./fs my_test.txt

valgrind: fs
	valgrind --tool=memcheck --leak-check=yes -s ./fs my_test.txt
