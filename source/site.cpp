#include "main.h"

static sitei sites[] = {{BRICK, 2, {Kobold, Leech}, 5, 0},
{BRICK, 3, {Skeleton, Zombie}, 0, 10},
{BRICK, 2, {Skeleton, Ghoul}, 0, 10},
{BRICK, 1, {Zombie, Ghoul}, 0, 10},
{}};

unsigned sitei::getleveltotal() const {
	unsigned r = 0;
	for(auto p = this; *p; p++)
		r += p->levels;
	return r;
}