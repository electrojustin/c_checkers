#include "search.h"
#include "move_gen.h"

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

struct alpha_beta_args
{
	char** game_board;
	enum COLOR current_color;
	int* up_max;
	int up_current;
	int max_depth;
	struct move* to_eval;
	sem_t* mutex;
};

sem_t num_threads;

char** make_move (char** current_board, struct move to_make)
{
	int i;
	int j;
	char col_dir;
	char row_dir;
	char** ret = (char**)malloc(8 * sizeof(char*));

	for (i = 0; i < 8; i++)
		ret[i] = (char*)malloc(8 * sizeof(char));

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
			ret [i][j] = current_board [i][j];
	}

	if (to_make.end.col - to_make.start.col < 0)
		col_dir = -1;
	else
		col_dir = 1;

	if (to_make.end.row - to_make.start.row < 0)
		row_dir = -1;
	else
		row_dir = 1;

	j = to_make.start.col;
	for (i = to_make.start.row; i != to_make.end.row; i = i + row_dir)
	{
		ret [i][j] = ' ';
		j = j + col_dir;
	}

	ret [i][j] = current_board [to_make.start.row][to_make.start.col];

	return ret;
}

struct move* alpha_beta (char** game_board, enum COLOR current_color, int* up_max, int up_current, int depth, int max_depth)
{
	struct move* move_list = NULL;
	struct move* current;
	struct move* tmp;
	struct move* best_current_move = NULL;
	struct move* best_opponent_move;
	char** new_game_board;
	enum COLOR next_color;
	int i;

	if (current_color == red)
		next_color = white;
	else
		next_color = red;

	get_legal_moves (&move_list, game_board, current_color);

	current = move_list;
	while (current)
	{
		if (depth < max_depth)
		{
			new_game_board = make_move (game_board, *current);
			best_opponent_move = alpha_beta (new_game_board, next_color, NULL, 0, depth + 1, max_depth);
			current->points -= best_opponent_move->points;

			for (i = 0; i < 8; i++)
				free(new_game_board[i]);
			free(new_game_board);
			free(best_opponent_move);
		}

		if (!best_current_move || best_current_move->points < current->points)
			best_current_move = current;

		current = current->next;
	}

	while (current)
	{
		tmp = current->next;

		if (best_current_move != current)
			free(current);

		current = tmp;
	}

	return best_current_move;
}

void* threaded_alpha_beta (void* args)
{
	//The initial call to alpha_beta that handles threading stuff
}

struct move get_best_move (char** game_board, enum COLOR current_color, int max_threads, int max_depth)
{
	struct move* best_move;
	struct move ret;

	best_move = alpha_beta (game_board, current_color, NULL, 0, 0, max_depth);
	if (!best_move)
	{
		ret.start.col = -1;
		ret.start.row = -1;
		ret.end.col = -1;
		ret.end.row = -1;
		ret.points = -1;
		ret.next = NULL;
		ret.prev = NULL;
	}
	else
	{
		ret = *best_move;
		free(best_move);
	}
	return ret;
}
