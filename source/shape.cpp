#include "main.h"

BSDATA(shapei) = {
	{"ShapeCorner", {4, 4},
	"UUUU"
	"X0XX"
	"X12X"
	"XXXX"},
	{"ShapeRoom", {5, 6},
	"UUUUU"
	"XX0XX"
	"X...X"
	"X.1.X"
	"X.2.X"
	"XXXXX"},
	{"ShapeRoomLarge", {5, 7},
	"UUUUU"
	"XX0XX"
	"X...X"
	"X...X"
	"X.1.X"
	"X.2.X"
	"XXXXX"},
	{"ShapeDeadEnd", {3, 3},
	"UUU"
	"X0X"
	"XXX"},
	{"ShapeHall", {9, 7},
	"UUUUUUUUU"
	"XXXX0XXXX"
	"X.......X"
	"X.......X"
	"X.......X"
	"X.......X"
	"XXXXXXXXX"},
};
assert_enum(shape, ShapeHall)

const char* shapei::getvertical() const {
	static char temp_data[8 * 8];
	auto sc = size.x;
	for(auto y = 0; y < size.y; y++) {
		for(auto x = 0; x < size.x; x++)
			temp_data[x * sc + y] = data[y * sc + x];
	}
	return temp_data;
}