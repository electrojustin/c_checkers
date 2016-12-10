all: test
test: color.o move_gen.o search.o main.o
	gcc -g color.o move_gen.o search.o main.o -o test
main.o: main.c search.h move.h color.h
	gcc -g -c main.c
search.o: search.h search.c move_gen.h move.h color.h
	gcc -g -c search.c
color.o: color.c color.h
	gcc -g -c color.c
move_gen.o: move_gen.c move_gen.h move.h color.h
	gcc -g -c move_gen.c 
clean:
	rm color.o move_gen.o search.o main.o
