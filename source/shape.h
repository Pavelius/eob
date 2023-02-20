#include "direction.h"
#include "point.h"

#pragma once

enum shape_s : unsigned char {
	ShapeCorner, ShapeRoom, ShapeRoomLarge, ShapeDeadEnd, ShapeHall,
};
struct shapei {
	const char*			id;
	point				size;
	const char*			data;
	const char*			getvertical() const;
	const char*			get(direction_s d, point& result_size) const;
};
