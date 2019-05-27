#include "rect.h"
#include "point.h"
#include "main.h"

static_assert(sizeof(dungeon::groundi) == 8, "laying items can be 8 bytes!");
static unsigned short	path_stack[256];
static unsigned char	path_push;
static unsigned char	path_pop;
const unsigned char		CellMask = 0x1F;

static void snode(unsigned short index, short unsigned* pathmap, short unsigned cost) {
	if(index==Blocked)
		return;
	auto a = pathmap[index];
	if(a == Blocked)
		return;
	if(a && cost >= a)
		return;
	path_stack[path_push++] = index;
	pathmap[index] = cost;
}

int dungeon::getitemside(item* pi) {
	if(((groundi*)pi) < items
		|| ((groundi*)pi) > (items + sizeof(items) / sizeof(items[0])))
		return 0;
	return ((groundi*)pi)->side;
}

unsigned dungeon::getitems(item** result, item** result_maximum, short unsigned index, int side) {
	auto p = result;
	for(auto& e : items) {
		if(!e.value)
			continue;
		if(e.index != index)
			continue;
		if(side != -1 && e.side != side)
			continue;
		if(p < result_maximum)
			*p++ = &e.value;
	}
	*p = 0;
	return p - result;
}

unsigned dungeon::getitems(item** result, item** result_maximum, overlayi* povr) {
	auto p = result;
	for(auto& e : cellar_items) {
		if(!e)
			continue;
		if(e.storage != povr)
			continue;
		if(p < result_maximum)
			*p++ = &e;
	}
	return p - result;
}

static dungeon::groundi& new_item(dungeon* pe) {
	for(auto& e : pe->items) {
		if(!e.value)
			return e;
	}
	return pe->items[0];
}

void dungeon::dropitem(short unsigned index, item rec, int side) {
	groundi& e = new_item(this);
	e.value = rec;
	e.index = index;
	e.side = side;
	e.flags = 0;
}

cell_s dungeon::get(short unsigned index) const {
	if(index == Blocked)
		return CellWall;
	return (cell_s)((data[index] & CellMask));
}

direction_s dungeon::getpassable(short unsigned index, direction_s* dirs) {
	for(int i = 0; dirs[i]; i++) {
		if(isblocked(moveto(index, dirs[i])))
			continue;
		return dirs[i];
	}
	return Center;
}

cell_s dungeon::gettype(cell_s id) {
	switch(id) {
	case CellSecrectButton:
	case CellPortal:
		return CellWall;
	default:
		return id;
	}
}

cell_s dungeon::get(int x, int y) const {
	if(x >= mpx || y >= mpy || x < 0 || y < 0)
		return CellWall;
	return get(location.getindex(x, y));
}

bool dungeon::is(short unsigned index, cell_flag_s value) const {
	if(index == Blocked)
		return false;
	return (data[index] & (0x80 >> value)) != 0;
}

void dungeon::set(short unsigned index, cell_flag_s value) {
	if(index == Blocked)
		return;
	data[index] |= (0x80 >> value);
}

void dungeon::set(short unsigned  index, cell_s value) {
	if(index == Blocked)
		return;
	data[index] = (data[index] & (~CellMask)) | value;
}

void dungeon::remove(short unsigned index, cell_flag_s value) {
	if(index == Blocked)
		return;
	data[index] &= ~(0x80 >> value);
}

void dungeon::remove(overlayi* po) {
	if(!po)
		return;
	po->index = Blocked;
	po->dir = Center;
	po->active = false;
}

dungeon::overlayi* dungeon::setoverlay(short unsigned index, cell_s type, direction_s dir) {
	if(index == Blocked)
		return 0;
	for(auto& e : overlays) {
		if(e.index==Blocked) {
			e.index = index;
			e.type = type;
			e.dir = dir;
			return &e;
		}
	}
	return 0;
}

void dungeon::add(overlayi* po, item it) {
	for(auto& e : cellar_items) {
		if(!e) {
			*((item*)&e) = it;
			e.storage = po;
			return;
		}
	}
}

void dungeon::remove(overlayi* po, item it) {
	for(auto& e : cellar_items) {
		if(e==it && e.storage==po) {
			e.clear();
			e.storage = 0;
			return;
		}
	}
}

dungeon::overlayi* dungeon::getoverlay(short unsigned index, direction_s dir) {
	for(auto& e : overlays) {
		if(!e.type)
			break;
		if(e.dir == dir
			&& e.index == index)
			return &e;
	}
	return 0;
}

bool dungeon::isactive(overlayi* po) {
	if(!po)
		return false;
	return po->active;
}

void dungeon::setactive(short unsigned index, bool value) {
	if(value)
		set(index, CellActive);
	else
		remove(index, CellActive);
}

void dungeon::setactive(short unsigned index, bool value, int radius) {
	auto x2 = gx(index) + 1;
	auto y2 = gy(index) + 1;
	for(int y = gy(index) - 1; y <= y2; y++) {
		if(y < 0 || y >= mpx)
			continue;
		for(int x = gx(index) - 1; x <= x2; x++) {
			if(x < 0 || x >= mpx)
				continue;
			setactive(getindex(x, y), value);
		}
	}
}

void dungeon::setactive(overlayi* po, bool value) {
	if(!po)
		return;
	auto wall = moveto(po->index, po->dir);
	auto left = moveto(wall, rotateto(po->dir, Left));
	auto right = moveto(wall, rotateto(po->dir, Right));
	if(po->active != value) {
		po->active = value;
		switch(po->type) {
		case CellPuller:
			setactive(po->index, value, 1);
			break;
		case CellDoorButton:
			setactive(moveto(po->index, po->dir), value);
			break;
		case CellKeyHole1:
		case CellKeyHole2:
			if(value) {
				if(get(left) == CellDoor)
					setactive(left, true);
				else if(get(right) == CellDoor)
					setactive(right, true);
			}
			break;
		}
	}
}

cell_s dungeon::gettype(overlayi* po) {
	if(!po)
		return CellPassable;
	return po->type;
}

void dungeon::fill(short unsigned index, int sx, int sy, cell_s value) {
	auto x1 = gx(index);
	auto x2 = x1 + sx;
	auto y1 = gy(index);
	auto y2 = y1 + sy;
	for(auto y = y1; y < y2; y++) {
		for(auto x = x1; x < x2; x++)
			set(getindex(x, y), value);
	}
}

void dungeon::clear() {
	memset(this, 0, sizeof(*this));
	overland_index = Blocked;
	stat.down.index = Blocked;
	stat.up.index = Blocked;
	stat.portal.index = Blocked;
	for(auto& e : overlays)
		e.index = Blocked;
}

void dungeon::finish(cell_s t) {
	for(int i = 1; i < mpx*mpy; i++)
		if(data[i] == CellUnknown)
			data[i] = t;
}

bool dungeon::isblocked(short unsigned index) {
	if(index == Blocked)
		return true;
	switch(get(index)) {
	case CellWall:
	case CellPortal:
		return true;
	case CellDoor:
		return !is(index, CellActive);
	}
	return false;
}

short unsigned get_nearest(dungeon* pd, short unsigned index, cell_s t1) {
	auto t = pd->get(index);
	if(t == t1)
		return index;
	return 0;
}

short unsigned dungeon::getnearest(short unsigned index, int radius, cell_s t1) {
	int x0 = gx(index);
	int y0 = gy(index);
	for(auto r = 0; r < radius; r++) {
		auto y1 = y0 - r;
		for(int x = x0 - r; x <= x0 + r; x++) {
			auto n = get_nearest(this, getindex(x, y1), t1);
			if(n)
				return n;
		}
		auto y2 = y0 + r;
		for(int x = x0 - r; x <= x0 + r; x++) {
			auto n = get_nearest(this, getindex(x, y2), t1);
			if(n)
				return n;
		}
		auto x1 = x0 - r;
		for(int y = y0 - r + 1; y < y2; y++) {
			auto n = get_nearest(this, getindex(x1, y), t1);
			if(n)
				return n;
		}
		auto x2 = x0 + r;
		for(int y = y0 - r + 1; y < y2; y++) {
			auto n = get_nearest(this, getindex(x2, y), t1);
			if(n)
				return n;
		}
	}
	return 0;
}

short unsigned* dungeon::getnearestfree(short unsigned* indicies, short unsigned index) {
	auto p = indicies;
	for(auto d = Left; d <= Down; d = (direction_s)(d + 1)) {
		auto i = moveto(index, d);
		if(isblocked(i))
			continue;
		*p++ = i;
	}
	*p = 0;
	return indicies;
}

short unsigned dungeon::random(short unsigned* indicies) {
	auto n = zlen(indicies);
	if(!n)
		return 0;
	return indicies[rand() % n];
}

bool dungeon::ismatch(short unsigned index, cell_s t1, cell_s t2) {
	if(!index)
		return true;
	auto t = dungeon::get(index);
	return t == t1 || t == t2;
}

bool dungeon::allaround(short unsigned index, cell_s t1, cell_s t2) {
	if(!index)
		return false;
	for(auto d = Left; d <= Down; d = (direction_s)(d + 1)) {
		if(ismatch(moveto(index, d), t1, t2))
			continue;
		return false;
	}
	return true;
}

void dungeon::turnto(short unsigned index, direction_s dr) {
	if(!dr)
		return;
	if(index == game::getcamera())
		game::setcamera(index, dr);
	else {
		creature* result[4]; getmonsters(result, index, game::getdirection());
		for(auto pc : result) {
			if(!pc)
				continue;
			pc->set(dr);
			pc->setmoved(true);
		}
	}
}

void dungeon::getmonsters(creature** result, short unsigned index, direction_s dr) {
	result[0] = result[1] = result[2] = result[3] = 0;
	if(!index)
		return;
	for(auto& e : monsters) {
		if(!e)
			continue;
		if(e.getindex() != index)
			continue;
		int side = game::getside(e.getside(), dr);
		result[side] = &e;
	}
}

bool dungeon::ismonster(short unsigned index) {
	creature* result[4]; getmonsters(result, index, Up);
	return result[0] || result[1] || result[2] || result[3];
}

int dungeon::getfreeside(short unsigned index) {
	creature* sides[4]; getmonsters(sides, index, Center);
	return game::getfreeside(sides);
}

void dungeon::getblocked(short unsigned* pathmap, bool treat_door_as_passable) {
	for(unsigned short index = 0; index < mpx*mpy; index++) {
		switch(get(index)) {
		case CellWall:
		case CellPortal:
		case CellUnknown:
			pathmap[index] = Blocked;
			break;
		case CellDoor:
			if(!treat_door_as_passable && !is(index, CellActive))
				pathmap[index] = Blocked;
			else
				pathmap[index] = 0;
			break;
		default:
			pathmap[index] = 0;
			break;
		}
	}
}

void dungeon::makewave(short unsigned start, short unsigned* pathmap) {
	if(start==Blocked || !pathmap)
		return;
	path_push = 0;
	path_pop = 0;
	path_stack[path_push++] = start;
	pathmap[start] = 1;
	while(path_push != path_pop) {
		auto pos = path_stack[path_pop++];
		auto cost = pathmap[pos] + 1;
		if(cost >= 0xFF00)
			break;
		snode(moveto(pos, Left), pathmap, cost);
		snode(moveto(pos, Right), pathmap, cost);
		snode(moveto(pos, Up), pathmap, cost);
		snode(moveto(pos, Down), pathmap, cost);
	}
}

short unsigned dungeon::getsecret() const {
	for(auto& e : overlays) {
		if(!e.type)
			break;
		if(!e.index)
			continue;
		if(e.type != CellSecrectButton)
			continue;
		return moveto(e.index, e.dir);
	}
	return 0;
}

item_s dungeon::getkeytype(cell_s keyhole) const {
	if(keyhole == CellKeyHole2)
		return stat.keys[1];
	return stat.keys[0];
}

void dungeon::setcontent(resource_s type, int level) {
	auto n = level / 2;
	if(n > 2)
		n = 2;
	this->type = type;
	this->level = level;
	switch(type) {
	case BRICK:
		stat.habbits[0] = Kobold;
		stat.habbits[1] = Leech;
		break;
	default:
		stat.habbits[0] = Goblin;
		stat.habbits[1] = Orc;
		break;
	}
	switch(type) {
	case BLUE:
		stat.keys[0] = KeySilver;
		stat.keys[1] = KeyMoon;
		break;
	case SILVER:
		stat.keys[0] = KeyDiamond;
		stat.keys[1] = KeySilver;
		break;
	default:
		stat.keys[0] = KeySilver;
		stat.keys[1] = KeyCooper;
		break;
	}
}

race_s dungeon::getlanguage() const {
	switch(type) {
	case BLUE: return Dwarf;
	case GREEN: case DROW: return Elf;
	default: return Human;
	}
}

short unsigned dungeon::getindex(int x, int y) const {
	if(x < 0 || x >= mpx || y < 0 || y >= mpy)
		return Blocked;
	return (short unsigned)(y * mpx + x);
}

short unsigned dungeon::gettarget(short unsigned index, direction_s dir) {
	for(int i = 0; i < 3; i++) {
		index = moveto(index, dir);
		if(!index)
			break;
		if(location.isblocked(index))
			return Blocked;
		if(location.ismonster(index))
			return index;
	}
	return Blocked;
}

void dungeon::traplaunch(short unsigned index, direction_s dir, item_s show, effecti& e) {
	while(index!=Blocked) {
		index = moveto(index, dir);
		if(index==Blocked)
			break;
		if(location.isblocked(index))
			return;
		if(location.ismonster(index)) {
		}
	}
}