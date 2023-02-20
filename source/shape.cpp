#include "crt.h"
#include "shape.h"

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
	"X...X"
	"X.1.X"
	"XXXXX"},
	{"ShapeRoomLarge", {5, 7},
	"UUUUU"
	"XX0XX"
	"X...X"
	"X...X"
	"X...X"
	"X.1.X"
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
assert_enum(shapei, ShapeHall)

const char* shapei::getvertical() const {
	static char temp_data[8 * 8];
	auto sc = size.x;
	for(auto y = 0; y < size.y; y++) {
		for(auto x = 0; x < size.x; x++)
			temp_data[x * sc + y] = data[y * sc + x];
	}
	return temp_data;
}

const char* shapei::get(direction_s d, point& result_size) const {
	static char temp_data[16 * 16];
	result_size = size;
	switch(d) {
	case Down:
		for(auto y = 0; y < size.y; y++) {
			for(auto x = 0; x < size.x; x++)
				temp_data[(size.y - y - 1) * size.x + x] = data[y * size.x + x];
		}
		break;
	case Right:
		iswap(result_size.x, result_size.y);
		for(auto y = 0; y < size.y; y++) {
			for(auto x = 0; x < size.x; x++)
				temp_data[x * result_size.x + (result_size.x - 1 - y)] = data[y * size.x + x];
		}
		break;
	case Left:
		iswap(result_size.x, result_size.y);
		for(auto y = 0; y < size.y; y++) {
			for(auto x = 0; x < size.x; x++)
				temp_data[x * result_size.x + y] = data[y * size.x + x];
		}
		break;
	default: return data;
	}
	return temp_data;
}