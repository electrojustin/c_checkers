#ifndef MOVE_H
#define MOVE_H
struct coord
{
	int row;
	int col;
};

struct move
{
	struct coord start;
	struct coord end;
	int points;
	struct move* next;
	struct move* prev;
};
#endif
