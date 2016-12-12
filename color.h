#ifndef COLOR_H
#define COLOR_H
enum COLOR
{
	red = 2,
	white = 7
};

char check_color (char piece, enum COLOR current_color); //Returns true if the piece is of color current_color, false otherwise. Note: this means spaces will always return false
#endif
