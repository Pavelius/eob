#include "main.h"

BSDATA(shapei) = {{"ShapeCorner", {4, 4}, {3, 4},
"UUUU"
"X0XX"
"X12X"
"XXXX",
"XXX"
"X2X"
"01X"
"XXX"},
{"ShapeRoom", {5, 6}, {6, 5},
"UUUUU"
"XX0XX"
"X...X"
"X.1.X"
"X.2.X"
"XXXXX",
"UXXXXX"
"UX...X"
"U0.12X"
"UX...X"
"UXXXXX"},
{"ShapeRoomLarge", {5, 7}, {7, 5},
"UUUUU"
"XX0XX"
"X...X"
"X...X"
"X.1.X"
"X.2.X"
"XXXXX",
"UXXXXXX"
"UX....X"
"U0..12X"
"UX....X"
"UXXXXXX"},
{"ShapeDeadEnd", {3, 3}, {3, 3},
"UUU"
"X0X"
"XXX",
"UXX"
"U0X"
"UXX"},
{"ShapeHall", {9, 7}, {10, 9},
"UUUUUUUUU"
"XXXX0XXXX"
"X.......X"
"X.......X"
"X.......X"
"X.......X"
"XXXXXXXXX",
"XXXXXXXXXU"
"X.......XU"
"X.......0U"
"X.......XU"
"X.......XU"
"XXXXXXXXXU"},
};
assert_enum(shape, ShapeHall)