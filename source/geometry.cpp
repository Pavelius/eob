#include "main.h"

static const direction_s rotate_direction[4][4] = {{Down, Left, Up, Right},
{Left, Up, Right, Down},
{Up, Right, Down, Left},
{Right, Down, Left, Up},
};

direction_s rotateto(direction_s d, direction_s d1) {
	return rotate_direction[d - Left][d1 - Left];
}

direction_s pointto(short unsigned from, short unsigned to) {
	static direction_s dirs[] = {Left, Right, Up, Down};
	for(auto e : dirs) {
		if(moveto(from, e) == to)
			return e;
	}
	return Center;
}

direction_s devectorized(direction_s dr, direction_s d) {
	switch(dr) {
	case Right:
		switch(d) {
		case Right: return Up;
		case Left: return Down;
		case Up: return Left;
		default: return Right;
		}
	case Left:
		switch(d) {
		case Left: return Up;
		case Right: return Down;
		case Up: return Right;
		default: return Left;
		}
	case Down:
		switch(d) {
		case Up: return Down;
		case Down: return Up;
		case Left: return Right;
		default: return Left;
		}
	default:
		return d;
	}
}

direction_s vectorized(direction_s d, direction_s d1) {
	switch(d) {
	case Up:
		return d1;
	case Left:
		switch(d1) {
		case Left: return Down;
		case Up: return Left;
		case Right: return Up;
		case Down: return Right;
		}
		break;
	case Right:
		switch(d1) {
		case Left: return Up;
		case Up: return Right;
		case Right: return Down;
		case Down: return Left;
		}
		break;
	case Down:
		switch(d1) {
		case Left: return Right;
		case Up: return Down;
		case Right: return Left;
		case Down: return Up;
		}
		break;
	}
	return Center;
}

short unsigned moveto(short unsigned index, direction_s d) {
	if(index==Blocked)
		return Blocked;
	switch(d) {
	case Left:
		if(gx(index) == 0)
			return Blocked;
		return index - 1;
	case Right:
		if(gx(index) >= mpx - 1)
			return Blocked;
		return index + 1;
	case Up:
		if(gy(index) == 0)
			return Blocked;
		return index - mpx;
	case Down:
		if(gy(index) >= mpy - 1)
			return Blocked;
		return index + mpx;
	default:
		return Blocked;
	}
}