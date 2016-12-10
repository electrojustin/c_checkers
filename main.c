#include "search.h"
#include "move.h"
#include "color.h"

#include <stdlib.h>
#include <stdio.h>

int main (void)
{
	char* game_board[] = { "       w",
			       "     w  ",
			       "    w   ",
			       "        ",
			       "    w   ",
			       " r   r  ",
			       "        ",
			       "        " };
	struct move best_move;

	best_move = get_best_move (game_board, red, 5, 1);

	printf ("(%d, %d)-(%d, %d)\n", best_move.start.row, best_move.start.col, best_move.end.row, best_move.end.col);
	printf ("Points: %d\n", best_move.points);
	//TODO: load and parse input files
	//call the search function with the given parameters
	//output the chosen move
}
