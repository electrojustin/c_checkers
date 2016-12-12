#include "search.h"
#include "move.h"
#include "color.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char** argv)
{
	char** game_board;
	int i;
	int j;
	int max_depth;
	int num_threads;
	enum COLOR color;
	struct move best_move;
	FILE* in_file;
	char* __arg_string = (char*)malloc(100 * sizeof(char));
	char* arg_string = __arg_string;

	if (argc == 1)
		in_file = fopen ("input.txt", "r");
	else
		in_file = fopen (argv[1], "r");

	//Allocate space for the input game board
	game_board = (char**)malloc(8 * sizeof(char*));
	for (i = 0; i < 8; i++)
	{
		game_board[i] = (char*)malloc(10 * sizeof(char)); //The extra characters is to account for null terminators and newlines

		for (j = 0; j < 9; j++)
			game_board[i][j] = 0;
	}

	//Load the game board from the input file and print it out in a prettier format to the console
	printf(" -----------------\n");
	for (i = 0; i < 8; i++)
	{
		fgets(game_board[i], 10, in_file);
		printf("%d|", i+1);
		for (j = 0; j < 8; j++)
			printf("%c|", game_board[i][j]);
		printf("\n");
		printf(" -----------------\n");
	}
	printf(" ");
	for (i = 0; i < 8; i++)
		printf(" %c", 'h'-i);
	printf("\n");

	//Load the options line from the input file
	fgets(arg_string, 100, in_file);
	if (!strncmp(arg_string, "whi", 2)) //Determine what color we're supposed to be thinking as
		color = white;
	else
		color = red;
	arg_string += 4;
	max_depth = atoi(arg_string); //Get the cutoff ply-depth
	while (arg_string[0] != ' ')
		arg_string ++;
	arg_string ++;
	num_threads = atoi(arg_string); //Get the number of threads to use

	//Perform an alpha-beta search with the specified parameters 
	best_move = get_best_move(game_board, color, num_threads, max_depth);

	//Print the best move to the console, along with its point value
	printf("%c%d-%c%d\n", 'h'-best_move.start.col, best_move.start.row+1, 'h'-best_move.end.col, best_move.end.row+1);
	printf("Points: %d\n", best_move.points);

	//Cleanup
	fclose (in_file);
	free (__arg_string);
	for (i = 0; i < 8; i++)
		free(game_board[i]);
	free(game_board);
}
