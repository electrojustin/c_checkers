#ifndef MOVE_H
#define MOVE_H
struct coord
{
	int row;
	int col;
	struct coord* next;
	struct coord* prev;
};

struct move
{
	struct coord start;
	struct coord end;
	struct coord* waypoints;
	int points;
	struct move* next;
	struct move* prev;
};
#endif
