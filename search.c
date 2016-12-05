#include "search.h"
#include "move_gen.h"

#include <pthread.h>
#include <semaphore.h>

struct alpha_beta_args
{
	char** game_board;
	enum COLOR current_color;
	int up_max;
	int up_current;
	int max_depth;
	struct move* to_eval;
	sem_t* mutex;
}

sem_t num_threads;

char** make_move (char** current_board, struct move to_make)
{
	//TODO: Reallocate memory for the game board
	//Copy current game board
	//Modify new game board according to the move
	//Return the new game board after the move
}

struct move alpha_beta (char** game_board, enum COLOR current_color, int up_max, int up_current, int depth, int max_depth)
{
	//Recurse until ply depth is hit and use negamax optimized with alpha-beta pruning to evaluate the "worth" of the move
	//If ply depth is hit, stop recursing and just return the best move we have currently without thinking into the future
}

void* threaded_alpha_beta (void*)
{
	//The initial call to alpha_beta that handles threading stuff
}

struct move get_best_move (char** game_board, enum COLOR current_color, int max_threads)
{
	//Searches the game board for the best move for either red or white, depending on the parameter current_color
}
