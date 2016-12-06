#include "color.h"
#include "move.h"

#ifndef SEARCH_H
#define SEARCH_H
struct move get_best_move (char** game_board, enum COLOR current_color, int max_threads, int max_depth);
#endif
