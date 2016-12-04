#include "color.h"

char check_color (char piece, enum COLOR current_color)
{
	return (piece & 0x0F) == current_color; //The ASCII codes for capital and lower case letters share the same 4 least significant bits
}
