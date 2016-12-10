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

void free_waypoints (struct move* to_free)
{
	struct coord* current = to_free->waypoints;
	struct coord* tmp;

	while (current)
	{
		tmp = current->next;
		free(current);
		current = tmp;
	}
}

char** make_move (char** current_board, struct move to_make)
{
	int i;
	int j;
	char col_dir;
	char row_dir;
	char** ret = (char**)malloc(8 * sizeof(char*));
	struct coord* current_waypoint;

	for (i = 0; i < 8; i++)
		ret[i] = (char*)malloc(8 * sizeof(char));

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
			ret [i][j] = current_board [i][j];
	}

	ret [to_make.start.row][to_make.start.col] = ' ';
	current_waypoint = to_make.waypoints;
	while (current_waypoint)
	{
		ret [current_waypoint->row][current_waypoint->col] = ' ';
		current_waypoint = current_waypoint->next;
	}

	ret [to_make.end.row][to_make.end.col] = current_board [to_make.start.row][to_make.start.col];

	return ret;
}

struct move* alpha_beta (char** game_board, enum COLOR current_color, int up_max, int up_current, int depth, int max_depth)
{
	struct move* move_list = NULL;
	struct move* current;
	struct move* tmp;
	struct move* best_current_move = NULL;
	struct move* best_opponent_move;
	char** new_game_board;
	enum COLOR next_color;
	int i;
	int best_points = -99999;

	if (current_color == red)
		next_color = white;
	else
		next_color = red;

	get_legal_moves (&move_list, game_board, current_color);

	current = move_list;
	while (current)
	{
		if ((up_current - best_points) < up_max)
			break;

		if (depth < max_depth)
		{
			new_game_board = make_move (game_board, *current);
			best_opponent_move = alpha_beta (new_game_board, next_color, best_points, current->points, depth + 1, max_depth);
			if (best_opponent_move)
				current->points -= best_opponent_move->points;
			else
				current->points = 99999;

			for (i = 0; i < 8; i++)
				free(new_game_board[i]);
			free(new_game_board);
			free_waypoints(best_opponent_move);
			free(best_opponent_move);
		}

		if (!best_current_move || best_current_move->points < current->points)
		{
			best_current_move = current;
			best_points = current->points;
		}

		current = current->next;
	}

	current = move_list;

	while (current)
	{
		tmp = current->next;

		if (best_current_move != current)
		{
			free_waypoints(current);
			free(current);
		}

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

	best_move = alpha_beta (game_board, current_color, -99999, 0, 0, max_depth);
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
		free_waypoints(best_move);
		free(best_move);
	}
	return ret;
}
