#include "crt.h"
#include "intellegence.h"

intellegencei bsdata<intellegencei>::elements[] = {
	{"NoInt"},
	{"Animal", 1},
	{"Semi", 2, 4},
	{"Low", 5, 7},
	{"Ave", 8, 10},
	{"Very", 11, 12},
	{"High", 13, 14},
	{"Exeptional", 15, 16},
	{"Genius", 17, 18},
	{"Supra", 19, 20},
	{"Godlike", 21, 25},
};
assert_enum(intellegencei, Godlike)
BSDATAF(intellegencei)