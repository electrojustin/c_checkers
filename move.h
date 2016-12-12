#ifndef MOVE_H
#define MOVE_H
//Convenient struct for bundling rows and columns and allowing them to be put in a linked list
struct coord
{
	int row;
	int col;
	struct coord* next;
	struct coord* prev;
};

//Struct that stores information about moves
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
