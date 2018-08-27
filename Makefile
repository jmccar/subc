output: main.o v4prefix.o
	gcc *.o -o run.exe

v4prefix.o: v4prefix.c
	gcc -c v4prefix.c

main.o: main.c
	gcc -c main.c

.PHONY: clean
clean:
	rm *.o *.exe

.PHONY: deb
deb:
	@cat input.txt | xargs ./run.exe

.PHONY: col
col:
	@cat input.txt | xargs ./run.exe 2> /dev/null | column -s, -t
