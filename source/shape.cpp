#include "main.h"

BSDATA(shapei) = {{"CornerV", Up, {4, 4},
"UUUU"
"X0XX"
"X12X"
"XXXX"},
{"CornerH", Left, {3, 4},
"XXX"
"X2X"
"01X"
"XXX"},
{"RoomV", Up, {5, 6},
"UUUUU"
"XX0XX"
"X...X"
"X.1.X"
"X.2.X"
"XXXXX"},
{"RoomH", Left, {6, 5},
"UXXXXX"
"UX...X"
"U0.12X"
"UX...X"
"UXXXXX"},
{"RoomLargeV", Up, {5, 7},
"UUUUU"
"XX0XX"
"X...X"
"X...X"
"X.1.X"
"X.2.X"
"XXXXX"},
{"RoomLargeH", Left, {7, 5},
"UXXXXXX"
"UX....X"
"U0..12X"
"UX....X"
"UXXXXXX"},
{"StairsV", Up, {3, 3},
"UUU"
"X0X"
"XXX"},
{"StairsH", Left, {3, 3},
"UXX"
"U0X"
"UXX"},
};
assert_enum(shape, DeadEndH)

direction_s shapei::get(direction_s dir, unsigned flags) const {
	if(flags&CellMirrorV) {
		if(dir == Up)
			return Down;
	}
	if(flags&CellMirrorH) {
		if(dir == Left)
			return Right;
	}
	return dir;
}