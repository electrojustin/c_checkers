all: color.o move_gen.o
color.o: color.c color.h
	gcc -c color.c
move_gen.o: move_gen.c move_gen.h move.h color.h
	gcc -c move_gen.c 
clean:
	rm color.o move_gen.o
