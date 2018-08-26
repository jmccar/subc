output: main.o v4prefix.o
	gcc *.o -o run.exe

v4prefix.o: v4prefix.c
	gcc -c v4prefix.c

main.o: main.c
	gcc -c main.c

clean:
	rm *.o *.exe

make test:
	./run.exe 203.000.013.255/023 016.032.064.128/008
