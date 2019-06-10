#include "main.h"

static const direction_s rotate_direction[4][4] = {{Down, Left, Up, Right},
{Left, Up, Right, Down},
{Up, Right, Down, Left},
{Right, Down, Left, Up},
};

direction_s to(direction_s d, direction_s d1) {
	return rotate_direction[d - Left][d1 - Left];
}

short unsigned to(short unsigned index, direction_s d) {
	if(index == Blocked)
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

direction_s pointto(short unsigned from, short unsigned dest) {
	static direction_s dirs[] = {Left, Right, Up, Down};
	for(auto e : dirs) {
		if(to(from, e) == dest)
			return e;
	}
	return Center;
}

int rangeto(short unsigned i1, short unsigned i2) {
	if(i1 == Blocked || i2 == Blocked)
		return -1;
	auto x1 = gx(i1), y1 = gy(i1);
	auto x2 = gx(i2), y2 = gy(i2);
	return imax(iabs(x1 - x2), iabs(y1 - y2));
}