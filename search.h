#include "color.h"
#include "move.h"
#include "move_gen.h"

#ifndef SEARCH_H
#define SEARCH_H
struct move get_best_move (char** game_board, enum COLOR current_color);
int eval (char** game_board, struct move to_eval);
#endif
