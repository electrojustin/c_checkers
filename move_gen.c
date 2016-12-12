#include "move_gen.h"
#include <stdlib.h>
#include <stdio.h>

//Detect if any checkers can make a jump. If you can make a jump, you must make the jump
char jump_available (struct move* move_list)
{
	struct move* current = move_list;

	while (current)
	{
		if (current->waypoints)
			return 1;

		current = current->next;
	}

	return 0;
}

//Remove moves that aren't jumps since they aren't technically legal moves
void prune_non_jumps (struct move** move_list)
{
	struct move* current = *move_list;
	struct move* tmp;

	while (current)
	{
		if (!current->waypoints)
		{
			if (!current->prev)
			{
				tmp = current;
				*move_list = current->next;
				current->next->prev = NULL;
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

//Add a move with the specified starting and ending coordinates to the linked list of legal moves
void add_move (struct move** move_list, struct coord start, struct coord end, int points)
{
	struct move* new_move = (struct move*)malloc (sizeof(struct move));

	new_move->start = start;
	new_move->end = end;
	new_move->points = points;
	new_move->prev = NULL;
	new_move->waypoints = NULL;

	if (!*move_list)
		new_move->next = NULL;
	else
	{
		(*move_list)->prev = new_move;
		new_move->next = *move_list;
	}

	*move_list = new_move;
}

//Since the function to add jumps to the legal move list is recursive, it keeps track of waypoints for each of its calls on the stack.
//As such, we need to carefully copy the linked list of waypoints into heap memory when generating the final legal move
void copy_waypoints (struct coord** dest, struct coord* src)
{
	struct coord* current = src;
	struct coord* new_waypoint;

	while (current)
	{
		new_waypoint = (struct coord*)malloc(sizeof(struct coord));
		*new_waypoint = *current;
		new_waypoint->prev = NULL;

		if (!*dest)
			new_waypoint->next = NULL;
		else
		{
			(*dest)->prev = new_waypoint;
			new_waypoint->next = *dest;
		}

		*dest = new_waypoint;

		current = current->next;
	}
}

//Simple function to check whether a move is within boundaries. Returns true if it is, false otherwise
char check_bounds (struct coord end_coord)
{
	if (end_coord.row > 7 || end_coord.row < 0)
		return 0;

	if (end_coord.col > 7 || end_coord.col < 0)
		return 0;

	return 1;
}

//Recursive function to generate jump moves
void add_jumps (struct move** move_list, char** game_board, struct coord start, struct coord current, enum COLOR current_color, int points, struct coord* waypoints)
{
	char direction = -1 * ((5 - current_color) / 2);
	struct coord next;
	char made_jump = 0;

	current.next = waypoints; //Keep track of all jumps made so far in a stack based linked list called "waypoints". This will help us determine what pieces to remove after a jump
	waypoints = &current;

	//Try to jump to the right
	next.row = current.row + 2*direction;
	next.col = current.col + 2;
	if (check_bounds (next))
	{
		if (game_board [current.row+direction][current.col+1] != ' ' &&
		    !check_color (game_board [current.row+direction][current.col+1], current_color) &&
		    game_board [next.row][next.col] == ' ')
		{
			add_jumps (move_list, game_board, start, next, current_color, points + 1, waypoints);
			made_jump = 1;
		}
	}
	
	//Try to jump to the left
	next.row = current.row + 2*direction;
	next.col = current.col - 2;
	if (check_bounds (next))
	{
		if (game_board [current.row+direction][current.col-1] != ' ' &&
		    !check_color (game_board [current.row+direction][current.col-1], current_color) &&
		    game_board [next.row][next.col] == ' ')
		{
			add_jumps (move_list, game_board, start, next, current_color, points + 1, waypoints);
			made_jump = 1;
		}
	}

	//If we can make a jump, we must. If we can't, our journey ends here
	if (!made_jump && (start.row != current.row || start.col != current.col))
	{
		add_move (move_list, start, current, points); //Add the move to the list
		copy_waypoints (&((*move_list)->waypoints), waypoints); //Copy the waypoints
	}
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

			//Add a normal move to the right
			end.row = i + direction;
			end.col = j + 1;
			if (check_bounds (end) && game_board [i+direction][j+1] == ' ')
				add_move (move_list, start, end, 0);

			//Add a normal move to the left
			end.row = i + direction;
			end.col = j - 1;
			if (check_bounds (end) && game_board [i+direction][j-1] == ' ')
				add_move (move_list, start, end, 0);

			//Add any possible jumps
			start.next = NULL;
			add_jumps (move_list, game_board, start, start, current_color, 0, NULL);
		}
	}

	//If we can make a jump, we must. Prune non-jumps in this scenario
	if (jump_available(*move_list))
		prune_non_jumps(move_list);
}
