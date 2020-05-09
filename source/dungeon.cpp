#include "rect.h"
#include "point.h"
#include "main.h"

static_assert(sizeof(dungeon::groundi) == 8, "laying items can be 8 bytes");
static unsigned short	path_stack[256];
static unsigned char	path_push;
static unsigned char	path_pop;
const unsigned char		CellMask = 0x1F;

static void snode(unsigned short index, short unsigned* pathmap, short unsigned cost) {
	if(index == Blocked)
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
		if(!e)
			continue;
		if(e.index != index)
			continue;
		if(side != -1 && e.side != side)
			continue;
		if(p < result_maximum)
			*p++ = &e;
	}
	*p = 0;
	return p - result;
}

unsigned dungeon::getitems(item** result, item** result_maximum, overlayi* povr) {
	if(!povr)
		return 0;
	auto storage_index = povr - overlays;
	auto p = result;
	for(auto& e : cellar_items) {
		if(!e)
			continue;
		if(e.storage_index != storage_index)
			continue;
		if(p < result_maximum)
			*p++ = &e;
	}
	return p - result;
}

static dungeon::groundi& new_item(dungeon* pe) {
	for(auto& e : pe->items) {
		if(!e)
			return e;
	}
	return pe->items[0];
}

void dungeon::dropitem(short unsigned index, item rec, int side) {
	groundi& e = new_item(this);
	*static_cast<item*>(&e) = rec;
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
		if(isblocked(to(index, dirs[i])))
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

void dungeon::remove(indext index, cell_flag_s value) {
	if(index == Blocked)
		return;
	data[index] &= ~(0x80 >> value);
}

void dungeon::remove(overlayi* po) {
	if(!po)
		return;
	po->clear();
}

dungeon::overlayi* dungeon::add(short unsigned index, cell_s type, direction_s dir) {
	if(index == Blocked)
		return 0;
	for(auto& e : overlays) {
		if(e)
			continue;
		e.type = type;
		e.index = index;
		e.dir = dir;
		stat.overlays++;
		return &e;
	}
	return 0;
}

void dungeon::add(overlayi* po, item it) {
	if(!po)
		return;
	auto storage_index = po - overlays;
	for(auto& e : cellar_items) {
		if(!e) {
			*((item*)&e) = it;
			e.storage_index = storage_index;
			return;
		}
	}
}

void dungeon::remove(overlayi* po, item it) {
	if(!po)
		return;
	auto storage_index = po - overlays;
	for(auto& e : cellar_items) {
		if(e == it && e.storage_index == storage_index) {
			e.clear();
			e.storage_index = 0xFF;
			return;
		}
	}
}

dungeon::overlayi* dungeon::getlinked(indext index) {
	if(index == Blocked)
		return 0;
	for(auto& e : overlays) {
		if(!e)
			break;
		if(e.index_link == index)
			return &e;
	}
	return 0;
}

dungeon::overlayi* dungeon::getoverlay(short unsigned index, direction_s dir) {
	for(auto& e : overlays) {
		if(e && e.dir == dir && e.index == index)
			return &e;
	}
	return 0;
}

bool dungeon::isactive(const overlayi* po) {
	if(!po)
		return false;
	return po->is(Active);
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
	auto wall = to(po->index, po->dir);
	auto left = to(wall, to(po->dir, Left));
	auto right = to(wall, to(po->dir, Right));
	if(po->is(Active) != value) {
		if(value)
			po->set(Active);
		else
			po->remove(Active);
		switch(po->type) {
		case CellPuller:
			if(po->index_link != Blocked) {
				setactive(po->index_link, value);
				switch(get(po->index_link)) {
				case CellDoor:
					if(value)
						mslog("You hear door open nearby");
					else
						mslog("You hear door closed nearby");
					break;
				}
			}
			else
				setactive(po->index, value, 1);
			break;
		case CellDoorButton:
			setactive(to(po->index, po->dir), value);
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
	stat.crypt.index = Blocked;
	stat.crypt_button.index = Blocked;
	for(auto& e : stat.spawn)
		e = Blocked;
	for(auto& e : overlays)
		e.clear();
}

void dungeon::finish(cell_s t) {
	for(int i = 1; i < mpx*mpy; i++)
		if(data[i] == CellUnknown)
			data[i] = t;
}

bool dungeon::isblocked(short unsigned index) const {
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

bool dungeon::isblocked(short unsigned index, int side) const {
	for(auto& e : monsters) {
		if(!e)
			continue;
		if(e.getindex() == index && e.getside() == side)
			return true;
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
		auto i = to(index, d);
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
		return Blocked;
	return indicies[rand() % n];
}

bool dungeon::ismatch(short unsigned index, cell_s t1, cell_s t2) {
	if(index == Blocked)
		return true;
	auto t = dungeon::get(index);
	return t == t1 || t == t2;
}

bool dungeon::allaround(short unsigned index, cell_s t1, cell_s t2) {
	if(index == Blocked)
		return false;
	for(auto d = Left; d <= Down; d = (direction_s)(d + 1)) {
		if(ismatch(to(index, d), t1, t2))
			continue;
		return false;
	}
	return true;
}

void dungeon::turnto(short unsigned index, direction_s dr) {
	if(!dr)
		return;
	if(index == game.getcamera())
		game.setcamera(index, dr);
	else {
		creature* result[4]; getmonsters(result, index, game.getdirection());
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
	if(index == Blocked)
		return;
	for(auto& e : monsters) {
		if(!e)
			continue;
		if(e.getindex() != index)
			continue;
		if(e.getsize() == Large)
			result[2] = &e;
		else {
			int side = game.getside(e.getside(), dr);
			result[side] = &e;
		}
	}
}

bool dungeon::ismonster(short unsigned index) {
	creature* result[4]; getmonsters(result, index, Up);
	return result[0] || result[1] || result[2] || result[3];
}

int dungeon::getfreeside(creature** sides) {
	auto size = getsize(sides);
	if(size >= Large)
		return -1;
	for(int i = 0; i < 4; i++) {
		if(!sides[i])
			return i;
	}
	return -1;
}

int dungeon::getfreeside(short unsigned index) {
	creature* sides[4]; getmonsters(sides, index, Center);
	return getfreeside(sides);
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
	if(start == Blocked || !pathmap)
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
		snode(to(pos, Left), pathmap, cost);
		snode(to(pos, Right), pathmap, cost);
		snode(to(pos, Up), pathmap, cost);
		snode(to(pos, Down), pathmap, cost);
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
		return to(e.index, e.dir);
	}
	return 0;
}

item_s dungeon::getkeytype(cell_s keyhole) const {
	if(keyhole == CellKeyHole2)
		return head.keys[1];
	return head.keys[0];
}

race_s dungeon::getlanguage() const {
	return head.language;
}

short unsigned dungeon::getindex(int x, int y) const {
	if(x < 0 || x >= mpx || y < 0 || y >= mpy)
		return Blocked;
	return (short unsigned)(y * mpx + x);
}

short unsigned dungeon::gettarget(short unsigned index, direction_s dir) {
	for(int i = 0; i < 3; i++) {
		index = to(index, dir);
		if(index == Blocked)
			break;
		if(location.isblocked(index))
			return Blocked;
		if(location.ismonster(index))
			return index;
	}
	return Blocked;
}

void dungeon::traplaunch(short unsigned index, direction_s dir, item_s show, const combati& ci) {
	bool stop = false;
	creature* result[4];
	draw::animation::update();
	while(index != Blocked) {
		if(index == Blocked)
			break;
		if(location.isblocked(index))
			return;
		getmonsters(result, index, to(dir, Down));
		for(auto p : result) {
			if(p) {
				attack(ci, p);
				stop = true;
			}
		}
		for(auto v : party) {
			auto p = v.getcreature();
			if(p && p->getindex() == index) {
				attack(ci, p);
				stop = true;
			}
		}
		if(stop)
			return;
		if(isvisible(index))
			draw::animation::thrownstep(index, dir, show, Center, 80);
		index = to(index, dir);
	}
}

void dungeon::overlayi::clear() {
	memset(this, 0, sizeof(*this));
	index = index_link = Blocked;
}

void dungeon::passround() {
	unsigned char map[mpx*mpy] = {0};
	for(auto& e : monsters) {
		if(!e)
			continue;
		auto i = e.getindex();
		if(i == Blocked || map[i] > 0)
			continue;
		map[i]++;
	}
	for(auto v : party) {
		auto p = v.getcreature();
		if(!p || !(*p))
			continue;
		auto i = p->getindex();
		if(i == Blocked || map[i] > 0)
			continue;
		map[i]++;
	}
	for(auto& e : items) {
		if(!e)
			continue;
		if(map[e.index] > 0)
			continue;
		map[e.index]++;
	}
	for(short unsigned i = 0; i < mpx*mpy; i++) {
		auto t = get(i);
		if(t == CellButton) {
			auto new_active = map[i] > 0;
			auto active = is(i, CellActive);
			if(active != new_active && new_active) {
				auto po = getlinked(i);
				if(po) {
					switch(po->type) {
					case CellTrapLauncher:
						if(!po->is(Active)) {
							static combati fire = {AutoHit, Fire, 0, {1, 6}};
							location.traplaunch(po->index, to(po->dir, Down), FireThrown, fire);
						}
						break;
					}
				}
			}
			setactive(i, new_active);
		}
	}
}

void dungeon::attack(const combati& wi, creature* defender) const {
	auto ac = defender->getac();
	auto tohit = 20 - wi.bonus - (10 - ac);
	auto rolls = xrand(1, 20);
	auto hits = -1;
	auto crhit = 20 - wi.critical_range;
	tohit = imax(2, imin(20, tohit));
	if(rolls >= tohit || rolls >= crhit) {
		auto damage = wi.damage;
		hits = damage.roll();
		// RULE: crtitical hit can deflected
		if(rolls >= crhit) {
			// RULE: critical damage depends on weapon and count in dices
			if(!defender->roll(CriticalDeflect)) {
				damage.b = 0; // Only initial dice rolled second time
				damage.c += wi.critical_multiplier;
				hits += damage.roll();
			}
		}
	}
	// Show result
	if(hits != -1) {
		// RULE: when attacking sleeping creature she wake up!
		defender->remove(Sleep);
		defender->damage(wi.type, hits);
	}
}

void dungeon::stop(short unsigned index) {
	creature* s_side[4]; getmonsters(s_side, index, Center);
	for(auto pc : s_side) {
		if(!pc)
			continue;
		pc->setmoved(true);
	}
}

size_s dungeon::getsize(creature** source) {
	auto result = Tiny;
	for(int i = 0; i < 4; i++) {
		if(!source[i])
			continue;
		auto v = source[i]->getsize();
		if(result < v)
			result = v;
	}
	return result;
}

static bool is_valid_move_by_size(creature** s_side, creature** d_side) {
	auto r = dungeon::getsize(s_side);
	if(r == Large)
		return !d_side[0] && !d_side[1] && !d_side[2] && !d_side[3];
	return true;
}

void dungeon::move(indext index, direction_s dr) {
	auto dest = to(index, dr);
	if(isblocked(dest))
		return;
	if(get(dest) == CellPit)
		return;
	if(dest == game.getcamera())
		return;
	if(index == dest) {
		stop(index);
		return;
	}
	creature* s_side[4]; getmonsters(s_side, index, Center);
	creature* d_side[4]; getmonsters(d_side, dest, Center);
	// Large monsters move only to free index
	if(!is_valid_move_by_size(s_side, d_side) || !is_valid_move_by_size(d_side, s_side))
		return;
	// Medium or smaller monsters can be mixed on different sides
	for(int i = 0; i < 4; i++) {
		auto pc = s_side[i];
		if(!pc)
			continue;
		pc->setmoved(true);
		auto s = i;
		if(d_side[s]) {
			s = getfreeside(d_side);
			if(s == -1)
				continue;
		}
		d_side[s] = pc;
		s_side[i] = 0;
		pc->setside(s);
		pc->setindex(dest);
		pc->set(dr);
	}
}

static void falling_damage() {
	for(auto v : party) {
		auto e = v.getcreature();
		if(!e)
			continue;
		// RULE: Climb walls helps when you drop down in pits
		if(e->roll(ClimbWalls))
			continue;
		e->damage(Bludgeon, dice::roll(3, 6));
	}
}

static void falling_landing() {
	creature* monsters[4];
	auto index = game.getcamera();
	location.getmonsters(monsters, index, game.getdirection());
	for(auto e : monsters) {
		if(!e)
			continue;
		e->clear();
	}
}

static void hearnoises(dungeon& location) {
	direction_s secret_dir = Center;
	auto index = game.getcamera();
	auto dir = game.getdirection();
	auto door_index = to(index, dir);
	if(door_index == Blocked || location.get(door_index) != CellDoor)
		return;
	if(location.is(door_index, CellActive))
		return;
	door_index = to(door_index, dir);
	if(door_index == Blocked)
		return;
	for(auto v : party) {
		auto pc = v.getcreature();
		if(!pc || !pc->isready())
			continue;
		int exp = 0;
		if(pc->get(Theif))
			exp = 50;
		if(pc->use(HearNoise, door_index, 0, 0, 50, false)) {
			creature* sides[4]; location.getmonsters(sides, door_index, Center);
			int count = 0;
			for(auto e : sides) {
				if(e)
					count++;
			}
			if(count) {
				if(count == 1 && sides[0] && sides[0]->getsize() >= Large)
					pc->say("There is something large behind this door", count);
				else if(count > 2)
					pc->say("Behind this door hide %1i creatures", count);
				else
					pc->say("Behind this door creature", count);
			} else
				pc->say("Nobody is behide this door");
			break;
		}
	}
}

void dungeon::rotate(direction_s direction) {
	auto i = game.getcamera();
	auto d = game.getdirection();
	game.setcamera(i, to(d, direction));
	hearnoises(*this);
}

void dungeon::clearboost() {
	auto pb = bsdata<boosti>::elements;
	for(auto& e : bsdata<boosti>()) {
		auto p = e.owner.getcreature();
		if(!p)
			continue;
		if(p->ishero())
			*pb++ = e;
		else
			p->update(e);
	}
	bsdata<boosti>::source.setcount(pb - bsdata<boosti>::begin());
}

void dungeon::move(direction_s direction) {
	int i = game.getcamera();
	int i1 = to(i, to(game.getdirection(), direction));
	auto t = get(i1);
	if(isblocked(i1) || ismonster(i1)
		|| ((t == CellStairsUp || t == CellStairsDown) && direction != Up)) {
		mslog("You can\'t go that way");
		return;
	}
	switch(t) {
	case CellStairsUp:
		mslog("Going up");
		clearboost();
		write();
		if(level <= 1) {
			// Leave dungeon
			draw::setnext(draw::mainmenu);
			return;
		}
		game.enter(overland_index, level - 1);
		game.setcamera(to(stat.down.index, stat.down.dir), stat.down.dir);
		game.write();
		break;
	case CellStairsDown:
		mslog("Going down");
		clearboost();
		write();
		game.enter(overland_index, level + 1);
		game.setcamera(to(stat.up.index, stat.up.dir), stat.up.dir);
		game.write();
		break;
	case CellPit:
		mslog("You falling down!");
		clearboost();
		write();
		game.setcamera(to(game.getcamera(), game.getdirection()));
		draw::animation::update();
		falling_damage();
		game.enter(overland_index, level + 1);
		falling_landing();
		game.write();
		break;
	default:
		mslog(0);
		game.setcamera(i1);
		hearnoises(*this);
		break;
	}
	game.endround();
}

static item* find_item_to_get(dungeon& location, short unsigned index, int side) {
	item* result[2];
	int count = location.getitems(result, zendof(result), game.getcamera(), side);
	if(!count)
		count = location.getitems(result, zendof(result), game.getcamera());
	if(!count)
		return 0;
	return result[0];
}

static int autodetect_side(dungeon& location, item* itm) {
	auto pc = game.getcreature(itm);
	if(!pc)
		return 0;
	int n = pc->getpartyindex();
	return n == -1 ? 0 : (n % 2);
}

void dungeon::pickitem(item* itm, int side) {
	if(!itm || *itm)
		return;
	if(side == -1)
		side = autodetect_side(*this, itm);
	auto gitm = find_item_to_get(*this, game.getcamera(), game.getside(side, game.getdirection()));
	if(!gitm)
		return;
	auto slot = game.getwear(itm);
	auto pc = game.getcreature(itm);
	if(!pc->isallow(*gitm, slot))
		return;
	if(!itm->stack(*gitm))
		iswap(*itm, *gitm);
	char temp[260]; stringbuilder sb(temp); itm->getname(sb);
	mslog("%1 picked up", temp);
}

void dungeon::dropitem(item* pi, int side) {
	auto pc = game.getcreature(pi);
	if(!pc)
		return;
	if(!pi || !(*pi))
		return;
	if(side == -1)
		side = autodetect_side(*this, pi);
	auto s1 = game.getwear(pi);
	if(!pc->isallowremove(*pi, s1, true))
		return;
	char temp[260]; stringbuilder sb(temp); pi->getname(sb);
	mslog("%1 dropped", temp);
	dropitem(game.getcamera(), *pi, game.getside(side, game.getdirection()));
	pi->clear();
}

unsigned dungeon::getmonstercount() const {
	auto result = 0;
	for(auto& e : monsters) {
		if(e)
			result++;
	}
	return result;
}

void dungeon::passhour() {
	short unsigned monster_count = getmonstercount();
	if(monster_count >= stat.monsters / 2)
		return;
	short unsigned source[2];
	source[0] = stat.spawn[0];
	source[1] = stat.spawn[1];
	zshuffle(source, sizeof(source) / sizeof(source[0]));
	for(auto index : source) {
		if(index == Blocked)
			continue;
		auto distance = rangeto(index, game.getcamera());
		if(distance <= 3)
			continue;
		addmonster(head.habbits[rand() % 2], index);
		break;
	}
}

void dungeon::set(short unsigned index, reaction_s v) {
	for(auto& e : monsters) {
		if(!e || e.getindex() != index)
			continue;
		e.set(v);
	}
}

void dungeon::formation(short unsigned index, direction_s dr) {
	creature* creatures[4]; getmonsters(creatures, index, dr);
	int sides[][2] = {{0, 2}, {1, 3}, {1, 2}, {0, 3}};
	for(auto& m : sides) {
		auto s1 = m[0];
		if(creatures[s1])
			continue;
		auto s2 = m[1];
		if(!creatures[s2])
			continue;
		auto pc = creatures[s2];
		creatures[s2] = creatures[s1];
		creatures[s1] = pc;
		pc->setside(game.getsideb(s1, dr));
	}
}

bool dungeon::is(short unsigned index, int width, int height, cell_s t1) const {
	if(index == Blocked)
		return false;
	auto x = gx(index), y = gy(index);
	for(auto x1 = x; x1 < x + width; x1++) {
		for(auto y1 = y; y1 < y + height; y1++) {
			auto t = get(get(x1, y1));
			if(t != t1)
				return false;
		}
	}
	return true;
}

bool dungeon::islineh(short unsigned index, direction_s dir, int count, cell_s t1, cell_s t2) const {
	if(index == Blocked)
		return false;
	auto i1 = index;
	auto i2 = index;
	while(count > 0) {
		i1 = to(i1, to(dir, Left));
		i2 = to(i2, to(dir, Right));
		if(i1 == Blocked || i2 == Blocked)
			return false;
		if(get(i1) != t1 && get(i1) != t2)
			return false;
		if(get(i2) != t1 && get(i2) != t2)
			return false;
		count--;
	}
	return true;
}

bool dungeon::isroom(short unsigned index, direction_s dir, int side, int height) const {
	while(height > 0) {
		if(!islineh(index, dir, side, CellWall, CellUnknown))
			return false;
		index = to(index, dir);
	}
	return true;
}

bool dungeon::is(const rect& rc, cell_s id) const {
	for(auto x = rc.x1; x <= rc.x2; x++) {
		for(auto y = rc.y1; y <= rc.y2; y++) {
			if(get(x, y) != id)
				return false;
		}
	}
	return true;
}

bool dungeon::create(rect& result, int w, int h) const {
	const int border = 1;
	auto x2 = mpx - w - 1 - border;
	auto y2 = mpy - h - 1 - border;
	auto x1 = xrand(border, x2);
	auto y1 = xrand(border, y2);
	for(auto y = y1; y < y2; y++) {
		for(auto x = x1; x < x2; x++) {
			rect rc = {x, y, x + w - 1, y + h - 1};
			if(is(rc, CellUnknown)) {
				result = rc;
				return true;
			}
		}
	}
	for(auto y = y1; y > border; y--) {
		for(auto x = x1; x > border; x--) {
			rect rc = {x, y, x + w - 1, y + h - 1};
			if(is(rc, CellUnknown)) {
				result = rc;
				return true;
			}
		}
	}
	return false;
}

void dungeon::makedoor(const rect& rc, overlayi& door, direction_s dir, bool has_button, bool has_button_on_other_side) {
	indext i = Blocked;
	switch(dir) {
	case Left: i = getindex(rc.x1, rc.y1 + rc.height() / 2); break;
	case Right: i = getindex(rc.x2, rc.y1 + rc.height() / 2); break;
	case Up: i = getindex(rc.x1 + rc.width() / 2, rc.y1); break;
	case Down: i = getindex(rc.x1 + rc.width() / 2, rc.y2); break;
	default: return;
	}
	door.index = i;
	door.dir = dir;
	set(i, CellDoor);
	if(has_button_on_other_side)
		add(to(i, to(dir, Down)), CellDoorButton, dir);
	if(has_button)
		add(to(i, dir), CellDoorButton, to(dir, Down));
}

void dungeon::makeroom(const rect& rc, overlayi& door) {
	static direction_s dirs[] = {Left, Right, Up, Down};
	// Floor
	for(auto x = rc.x1; x <= rc.x2; x++) {
		for(auto y = rc.y1; y <= rc.y2; y++)
			set(getindex(x, y), CellPassable);
	}
	// Walls
	for(auto x = rc.x1; x <= rc.x2; x++)
		set(getindex(x, rc.y1), CellWall);
	for(auto x = rc.x1; x <= rc.x2; x++)
		set(getindex(x, rc.y2), CellWall);
	for(auto y = rc.y1; y <= rc.y2; y++)
		set(getindex(rc.x1, y), CellWall);
	for(auto y = rc.y1; y <= rc.y2; y++)
		set(getindex(rc.x2, y), CellWall);
	makedoor(rc, door, maprnd(dirs), false, false);
}