#include "move_gen.h"
#include <stdlib.h>

char jump_available (struct move* move_list)
{
	struct move* current = move_list;

	while (current)
	{
		if (current->end.row - current->start.row > 1 || current->end.row - current->start.row < -1)
			return 1;

		current = current->next;
	}

	return 0;
}

void prune_non_jumps (struct move** move_list)
{
	struct move* current = *move_list;
	struct move* tmp;

	while (current)
	{
		if (current->end.row - current->start.row == 1 || current->end.row - current->start.row == -1)
		{
			if (!current->prev)
			{
				tmp = current;
				*move_list = current->next;
				current = current->next;
				free (tmp);
			}
			else
			{
				tmp = current;
				current->prev->next = current->next;
				if (current->next)
					current->next->prev = current->prev;
				current = current->next;
				free (tmp);
			}
		}
		else
			current = current->next;
	}
}

void add_move (struct move** move_list, struct coord start, struct coord end, int points)
{
	struct move* new_move = (struct move*)malloc (sizeof(struct move));

	new_move->start = start;
	new_move->end = end;
	new_move->points = points;
	new_move->prev = NULL;

	if (!*move_list)
		new_move->next = NULL;
	else
	{
		(*move_list)->prev = new_move;
		new_move->next = *move_list;
	}

	*move_list = new_move;
}

char check_bounds (struct coord end_coord)
{
	if (end_coord.row > 8 || end_coord.row < 0)
		return 0;

	if (end_coord.col > 8 || end_coord.col < 0)
		return 0;

	return 1;
}

void add_jumps (struct move** move_list, char** game_board, struct coord start, struct coord current, enum COLOR current_color, int points)
{
	char direction = (5 - current_color) / 2;
	struct coord next;
	char made_jump = 0;

	next.row = current.row + 2*direction;
	next.col = current.col + 2;
	if (check_bounds (next))
	{
		if (game_board [current.row+direction][current.col+1] != ' ' &&
		    !check_color (game_board [current.row+direction][current.col+1], current_color) &&
		    game_board [next.row][next.col] == ' ')
		{
			add_jumps (move_list, game_board, start, next, current_color, points + 1);
			made_jump = 1;
		}
	}
	
	next.row = current.row + 2*direction;
	next.col = current.col - 2;
	if (check_bounds (next))
	{
		if (game_board [current.row+direction][current.col-1] != ' ' &&
		    !check_color (game_board [current.row+direction][current.col-1], current_color) &&
		    game_board [next.row][next.col] == ' ')
		{
			add_jumps (move_list, game_board, start, next, current_color, points + 1);
			made_jump = 1;
		}
	}

	if (!made_jump && (start.row != current.row || start.col != current.col))
		add_move (move_list, start, current, points);
}

void get_legal_moves (struct move** move_list, char** game_board, enum COLOR current_color)
{
	int i;
	int j;
	char direction = -1 * ((5 - current_color) / 2); //Red goes up the board, so direction=-1. White goes down the board, so direction=1
	struct coord start;
	struct coord end;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (!check_color (game_board [i][j], current_color))
				continue;

			start.row = i;
			start.col = j;

			if (game_board [i+direction][j+1] == ' ')
			{
				end.row = i + direction;
				end.col = j + 1;
				if (check_bounds (end))
					add_move (move_list, start, end, 0);
			}

			if (game_board [i+direction][j-1] == ' ')
			{
				end.row = i + direction;
				end.col = j - 1;
				if (check_bounds (end))
					add_move (move_list, start, end, 0);
			}

			add_jumps (move_list, game_board, start, start, current_color, 0);
		}
	}
}
