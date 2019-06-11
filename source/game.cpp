#include "archive.h"
#include "main.h"

static unsigned short	camera_index = Blocked;
static direction_s		camera_direction;
static unsigned			overland_index = 1;
static unsigned char	location_level = 1;
creature*				game::party[7];
unsigned				game::rounds;
static unsigned			rounds_turn;
static unsigned			rounds_hour;
dungeon					location_above;
dungeon					location;
static creature			hero_data[32];

static const char* name_direction[] = {"floor",
"left", "forward", "right", "rear"};

short unsigned game::getcamera() {
	return camera_index;
}

direction_s game::getdirection() {
	return camera_direction;
}

void game::setcamera(short unsigned index, direction_s direction) {
	camera_index = index;
	if(direction != Center)
		camera_direction = direction;
	if(index == Blocked)
		return;
	int x = gx(index);
	int y = gy(index);
	location.set(index, CellExplored);
	location.set(location.getindex(x - 1, y), CellExplored);
	location.set(location.getindex(x + 1, y), CellExplored);
	location.set(location.getindex(x, y + 1), CellExplored);
	location.set(location.getindex(x, y - 1), CellExplored);
}

creature* creature::newhero() {
	for(auto& e : hero_data) {
		if(e)
			continue;
		return &e;
	}
	return hero_data;
}

bool creature::ishero() const {
	return this >= hero_data
		&& this <= (hero_data + sizeof(hero_data) / sizeof(hero_data[0]));
}

static int find_index(int** items, int* itm) {
	for(int i = 0; i < 12; i++)
		if(items[i] == itm)
			return i;
	return -1;
}

bool game::action::question(item* current_item) {
	char name[128];
	auto pc = gethero(current_item);
	if(!pc || pc->gethits() <= 0)
		return false;
	current_item->getname(name, zendof(name));
	pc->say("This is %1", name);
	return true;
}

static const char place_sides[4][4] = {{1, 3, 0, 2},
{0, 1, 2, 3},
{2, 0, 3, 1},
{3, 2, 1, 0},
};

int game::getside(int side, direction_s dr) {
	if(dr == Center)
		return side;
	return place_sides[dr - Left][side];
}

int game::getsideb(int side, direction_s dr) {
	if(dr == Center)
		return side;
	for(int i = 0; i < 4; i++) {
		if(place_sides[dr - Left][i] == side)
			return i;
	}
	return -1;
}

static void select_parcipants(creature** result, short unsigned index) {
	memset(result, 0, sizeof(result[0]) * 11);
	auto dr = game::getdirection();
	location.getmonsters(result, index, dr);
	auto p = result;
	for(int i = 0; i < 4; i++) {
		if(result[i])
			*p++ = result[i];
	}
	for(int i = 0; i < 6; i++) {
		if(game::party[i] && game::party[i]->isready())
			*p++ = game::party[i];
	}
	*p = 0;
}

static int compare_parcipants(const void* p1, const void* p2) {
	auto pc1 = *((creature**)p1);
	auto pc2 = *((creature**)p2);
	int i1 = pc1->getinitiative();
	int i2 = pc2->getinitiative();
	return i2 - i1;
}

void roll_inititative(creature** result) {
	for(int i = 0; result[i]; i++) {
		auto pc = result[i];
		int value = xrand(1, 10);
		value += pc->getspeed();
		pc->setinitiative(value);
		pc->setmoved(true);
	}
	int count = zlen(result);
	qsort(result, count, sizeof(result[0]), compare_parcipants);
}

static creature* get_best_enemy(creature** quarter, int* indecies) {
	for(int i = 0; i < 4; i++) {
		auto pc = quarter[indecies[i]];
		if(pc)
			return pc;
	}
	return 0;
}

void game::getheroes(creature** result, direction_s dr) {
	result[0] = result[1] = result[2] = result[3] = 0;
	for(auto pc : game::party) {
		if(!pc || pc->gethits() <= 0)
			continue;
		int side = pc->getside();
		if(side > 3)
			continue;
		result[side] = pc;
	}
}

creature* game::getdefender(short unsigned index, direction_s dr, creature* attacker) {
	creature* defenders[4];
	int attacker_side = attacker->getside();
	if(!attacker->ishero())
		attacker_side = game::getside(attacker_side, dr);
	if(!attacker->ishero()) {
		static int sides[2][4] = {{0, 1, 2, 3}, {1, 0, 3, 2}, };
		game::getheroes(defenders, dr);
		return get_best_enemy(defenders, sides[attacker_side % 2]);
	} else {
		static int sides[2][4] = {{2, 3, 0, 1}, {3, 2, 1, 0}};
		location.getmonsters(defenders, index, dr);
		return get_best_enemy(defenders, sides[attacker_side % 2]);
	}
}

void game::action::attack(short unsigned index_of_monsters, bool ranged) {
	creature* parcipants[13];
	auto dr = getdirection();
	location.turnto(index_of_monsters, to(dr, Down));
	location.formation(index_of_monsters, to(dr, Down));
	draw::animation::update();
	select_parcipants(parcipants, index_of_monsters);
	roll_inititative(parcipants);
	for(int i = 0; parcipants[i]; i++) {
		auto attacker = parcipants[i];
		if(!attacker->isready())
			continue;
		attacker->attack(index_of_monsters, dr, 0, ranged);
	}
	// RULE: Hasted units make second move at end of combat round
	for(int i = 0; parcipants[i]; i++) {
		auto attacker = parcipants[i];
		if(!attacker->isready())
			continue;
		if(attacker->is(Hasted)
			|| attacker->getbonus(OfSpeed, Legs)
			|| attacker->getbonus(OfSpeed, Elbow))
			attacker->attack(index_of_monsters, dr, 0, ranged);
	}
}

void read_message(dungeon* pd, dungeon::overlayi* po);

bool game::action::manipulate(item* itm, direction_s dr) {
	int index = getcamera();
	auto po = location.getoverlay(index, dr);
	if(!po)
		return false;
	auto pc = gethero(itm);
	switch(location.gettype(po)) {
	case CellSecrectButton:
		// RULE: secret doors gain experience
		creature::addexp(500, 0);
		location.set(to(index, dr), CellPassable);
		location.remove(po);
		pc->say("This is secret door");
		break;
	case CellCellar:
		if(*itm) {
			if(bsmeta<itemi>::elements[itm->gettype()].image.size == 1) {
				pc->say("This item does not fit in cellar");
			} else {
				location.add(po, *itm);
				itm->clear();
			}
		} else {
			item* items[1];
			if(location.getitems(items, items + sizeof(items) / sizeof(items[0]), po)) {
				*itm = *items[0];
				items[0]->clear();
			} else
				pc->say("There is nothing to grab");
		}
		return true;
	case CellMessage:
		po = location.getoverlay(index, dr);
		if(!po)
			return false;
		read_message(&location, po);
		break;
	case CellPuller:
	case CellDoorButton:
		po = location.getoverlay(index, dr);
		if(!po)
			return false;
		location.setactive(po, !location.isactive(po));
		break;
	case CellKeyHole1:
	case CellKeyHole2:
		pc->say("This is key hole");
		break;
	case CellTrapLauncher:
		if(pc->get(Theif)) {
			if(po->is(Active))
				pc->say("This is a disabled trap");
			else
				pc->say("This is a trap");
		} else
			pc->say("Some kind of strange mechanism");
		break;
	default:
		return false;
	}
	return true;
}

void game::action::thrown(item* itm) {
	static char place_sides[4][2] = {{3, 1},
	{2, 3},
	{0, 2},
	{1, 0},
	};
	if(!itm || !*itm)
		return;
	auto pc = gethero(itm);
	auto side = pc->getside() % 2;
	auto index = draw::animation::thrown(getcamera(), getdirection(),
		itm->gettype(), side ? Right : Left);
	location.dropitem(index, *itm, place_sides[getdirection() - Left][side]);
	itm->clear();
}

creature* game::gethero(item* itm) {
	for(auto e : party) {
		if(!e)
			break;
		auto p1 = e->getitem(FirstInvertory);
		auto p2 = e->getitem(LastInvertory);
		if(itm >= p1 && itm <= p2)
			return e;
	}
	return 0;
}

wear_s game::getitempart(item* itm) {
	auto p = gethero(itm);
	if(p)
		return (wear_s)(FirstInvertory + (itm - p->getitem(FirstInvertory)));
	return Head;
}

void game::passround() {
	// Походим за монстров
	for(auto& e : location.monsters) {
		if(!e || !e.isready() || e.ismoved())
			continue;
		auto party_index = game::getcamera();
		auto party_direct = game::getdirection();
		auto monster_index = e.getindex();
		auto monster_direct = e.getdirection();
		if(monster_index == Blocked)
			continue;
		if(e.is(Scared)) {
			direction_s free_directions[] = {to(party_direct, Up), to(party_direct, Left), to(party_direct, Left), Center};
			auto free_direct = location.getpassable(monster_index, free_directions);
			if(monster_direct != free_direct)
				location.turnto(monster_index, free_direct);
			if(free_direct)
				location.move(monster_index, free_direct);
		} else if(to(monster_index, monster_direct) == party_index)
			e.interract();
		else if(d100() < 45) {
			auto next_index = to(monster_index, monster_direct);
			if(location.isblocked(next_index) || d100() < 30) {
				short unsigned indicies[5];
				auto n = location.random(location.getnearestfree(indicies, monster_index));
				if(n != Blocked)
					location.turnto(monster_index, pointto(monster_index, n));
			} else
				location.move(monster_index, monster_direct);
		} else
			location.stop(monster_index);
	}
	// Regular monster and heroes update
	for(auto& e : location.monsters) {
		if(e)
			e.update(false);
	}
	for(auto pc : game::party) {
		if(pc)
			pc->update(true);
	}
	// Slow update
	while(rounds_turn < rounds) {
		for(auto& e : location.monsters) {
			if(e)
				e.update_turn(false);
		}
		for(auto pc : game::party) {
			if(pc)
				pc->update_turn(true);
		}
		rounds_turn += 10;
	}
	// Hourly update
	while(rounds_hour < rounds) {
		for(auto& e : location.monsters) {
			if(e)
				e.update_hour(false);
		}
		for(auto pc : game::party) {
			if(pc)
				pc->update_hour(true);
		}
		rounds_hour += 60;
		location.passhour();
	}
	location.passround();
}

bool get_secret(short unsigned index, direction_s sight_dir, direction_s rotate_dir, direction_s& secret_dir) {
	auto po = location.getoverlay(index, to(sight_dir, rotate_dir));
	if(!po)
		return false;
	if(po->type != CellSecrectButton)
		return false;
	secret_dir = rotate_dir;
	return true;
}

void game::findsecrets() {
	static const char* speech[] = {
		"I see something on %1 wall",
		"There is button to the %1",
	};
	direction_s secret_dir = Center;
	auto index = getcamera();
	auto dir = getdirection();
	if(!get_secret(index, dir, Left, secret_dir)
		&& !get_secret(index, dir, Right, secret_dir))
		return;
	for(auto pc : game::party) {
		if(!pc || !pc->isready())
			continue;
		if(pc->roll(DetectSecrets)) {
			pc->say(maprnd(speech), name_direction[secret_dir]);
			break;
		}
	}
}

void game::passtime(int minutes) {
	while(minutes > 0) {
		passround();
		auto count = 5;
		if(count > minutes)
			count = minutes;
		minutes -= count;
		game::rounds += count;
	}
}

void game::enter(unsigned short index, unsigned char level) {
	overland_index = index;
	location_level = level;
	location.clear();
	location_above.clear();
	if(!location.read(overland_index, location_level))
		return;
	if(location_level > 1)
		location_above.read(overland_index, location_level - 1);
	draw::settiles(location.head.type);
	if(camera_index == Blocked)
		game::setcamera(to(location.stat.up.index, location.stat.up.dir), location.stat.up.dir);
}

bool creature::set(skill_s skill, short unsigned index) {
	for(auto& e : location.events) {
		if(e.pc == this
			&& e.skill == skill
			&& e.index == index)
			return false;
		if(!e.pc) {
			e.pc = this;
			e.skill = skill;
			e.index = index;
			return true;
		}
	}
	return false;
}

static bool serialize(bool writemode) {
	io::file file("maps/gamedata.sav", writemode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive::dataset pointers[] = {hero_data};
	archive a(file, writemode, pointers);
	if(!a.signature("SAV"))
		return false;
	if(!a.version(0, 1))
		return false;
	a.set(overland_index);
	a.set(location_level);
	a.set(camera_index);
	a.set(camera_direction);
	a.set(game::rounds);
	a.set(rounds_turn);
	a.set(rounds_hour);
	a.set(hero_data);
	a.set(game::party);
	return true;
}

static char* fname(char* result, unsigned short index, int level) {
	zcpy(result, "maps/d");
	sznum(zend(result), index, 5, "00000", 10);
	sznum(zend(result), level, 2, "00", 10);
	zcat(result, ".aum");
	return result;
}

template<> void archive::set<dungeon::eventi>(dungeon::eventi& e) {
	set(e.pc);
	set(e.index);
	set(e.skill);
}
template<> void archive::set<dungeon::overlayitem>(dungeon::overlayitem& e) {
	set(*((item*)&e));
	set(e.storage);
}
template<> void archive::set<dungeon>(dungeon& e) {
	set(e.head);
	set(e.overland_index);
	set(e.level);
	set(e.stat);
	set(e.chance);
	set(e.data);
	set(e.items);
	set(e.overlays);
	set(e.monsters);
	set(e.cellar_items);
	set(e.events);
}

static bool serialize(dungeon& e, short unsigned overland_index, int level, bool write_mode) {
	char temp[260];
	io::file file(fname(temp, overland_index, level), write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive::dataset pointers[] = {hero_data, e.overlays};
	archive a(file, write_mode, pointers);
	a.set(e);
	return true;
}

void dungeon::write() {
	serialize(*this, overland_index, level, true);
}

bool dungeon::read(unsigned short overland_index, unsigned char level) {
	return serialize(*this, overland_index, level, false);
}

void game::write() {
	if(!serialize(true))
		return;
	if(location)
		location.write();
}

bool game::read() {
	if(!serialize(false))
		return false;
	game::enter(overland_index, location_level);
	return true;
}