#include "move.h"
#include "color.h"

#ifndef MOVE_GEN_H
#define MOVE_GEN_H
void get_legal_moves (struct move** move_list, char** game_board, enum COLOR current_color);
#endif
