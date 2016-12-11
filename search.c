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
	struct move** up_max;
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

void* threaded_alpha_beta (void* __args)
{
	struct alpha_beta_args* args = (struct alpha_beta_args*)__args;
	struct move* move_list = NULL;
	struct move* current;
	struct move* tmp;
	struct move* best_opponent_move;
	char** new_game_board;
	enum COLOR next_color;
	int i;
	int best_points = -99999;

	if (args->current_color == red)
		next_color = white;
	else
		next_color = red;

	get_legal_moves (&move_list, args->game_board, args->current_color);

	current = move_list;
	while (current)
	{
		sem_wait(args->mutex);
		if (*(args->up_max) && (args->to_eval->points - best_points) < (*(args->up_max))->points)
		{
			sem_post(args->mutex);
			break;
		}
		sem_post(args->mutex);

		if (args->max_depth > 1)
		{
			new_game_board = make_move (args->game_board, *current);
			best_opponent_move = alpha_beta (new_game_board, next_color, best_points, current->points, 2, args->max_depth);
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

		if (best_points < current->points)
			best_points = current->points;

		current = current->next;
	}

	current = move_list;

	while (current)
	{
		tmp = current->next;

		free_waypoints(current);
		free(current);

		current = tmp;
	}

	args->to_eval->points -= best_points;

	sem_wait(args->mutex);
	if (!*(args->up_max) || args->to_eval->points > (*(args->up_max))->points)
		*(args->up_max) = args->to_eval;
	sem_post(args->mutex);

	for (i = 0; i < 8; i++)
		free(args->game_board[i]);
	free(args->game_board);
	free(args);

	sem_post(&num_threads);

	pthread_detach(pthread_self());
	pthread_exit(NULL);
}

struct move get_best_move (char** game_board, enum COLOR current_color, int max_threads, int max_depth)
{
	struct move* move_list = NULL;
	struct move* best_move = NULL;
	struct move* current;
	struct move* tmp;
	struct move ret;
	struct alpha_beta_args* args;
	sem_t mutex;
	int current_avail_threads;
	enum COLOR next_color;
	char** new_game_board;
	pthread_t thread;
	int i;

	sem_init(&mutex, 0, 1);
	sem_init(&num_threads, 0, max_threads);

	get_legal_moves(&move_list, game_board, current_color);

	if (current_color == red)
		next_color = white;
	else
		next_color = red;

	current = move_list;
	while (current)
	{
		new_game_board = make_move (game_board, *current);

		args = (struct alpha_beta_args*)malloc(sizeof(struct alpha_beta_args));
		args->game_board = new_game_board;
		args->current_color = next_color;
		args->up_max = &best_move;
		args->max_depth = max_depth;
		args->to_eval = current;
		args->mutex = &mutex;

		sem_wait(&num_threads);

		pthread_create(&thread, NULL, threaded_alpha_beta, args);

		current = current->next;
	}

	do
	{
		sem_getvalue(&num_threads, &current_avail_threads);
	}
	while (current_avail_threads != max_threads);

	current = move_list;
	while (current)
	{
		tmp = current->next;

		if (current != best_move)
		{
			free_waypoints(current);
			free(current);
		}

		current = tmp;
	}
	
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
