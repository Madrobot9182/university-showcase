wordcount: distwc.o mapreduce.o threadpool.o partition.o
	gcc -Wall -Werror -pthread -o wordcount distwc.o mapreduce.o threadpool.o partition.o

threadpool.o:
	gcc -Wall -Werror -pthread -c threadpool.c

mapreduce.o:
	gcc -Wall -Werror -pthread -c mapreduce.c

distwc.o:
	gcc -Wall -Werror -pthread -c distwc.c

clean:
	rm -f wordcount *.o
	rm -rf result*.txt

runall:
	./wordcount sample_inputs/sample1.txt sample_inputs/sample2.txt sample_inputs/sample3.txt sample_inputs/sample4.txt sample_inputs/sample5.txt sample_inputs/sample6.txt sample_inputs/sample7.txt sample_inputs/sample8.txt sample_inputs/sample9.txt sample_inputs/sample10.txt sample_inputs/sample11.txt sample_inputs/sample12.txt sample_inputs/sample13.txt sample_inputs/sample14.txt sample_inputs/sample15.txt sample_inputs/sample16.txt sample_inputs/sample17.txt sample_inputs/sample18.txt sample_inputs/sample19.txt sample_inputs/sample20.txt

# Everything else below was for testing

testrun:
	gcc -Wall -pthread -std=c99 -g -o testmain testmain.c
	./testmain sample_inputs/sample9.txt sample_inputs/sample18.txt
	rm testmain

fulltestrun:
	gcc -Wall -pthread -std=c99 -g -o testmain testmain.c
	./testmain sample_inputs/sample1.txt sample_inputs/sample2.txt sample_inputs/sample3.txt sample_inputs/sample4.txt sample_inputs/sample5.txt sample_inputs/sample6.txt sample_inputs/sample7.txt sample_inputs/sample8.txt sample_inputs/sample9.txt sample_inputs/sample10.txt sample_inputs/sample11.txt sample_inputs/sample12.txt sample_inputs/sample13.txt sample_inputs/sample14.txt sample_inputs/sample15.txt sample_inputs/sample16.txt sample_inputs/sample17.txt sample_inputs/sample18.txt sample_inputs/sample19.txt sample_inputs/sample20.txt
	rm testmain
testvalgrind:
	gcc -Wall -pthread -std=c99 -g -o testmain testmain.c
	valgrind --tool=memcheck --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all ./testmain sample_inputs/sample20.txt

	rm testmain

testclean:
	rm testmain
