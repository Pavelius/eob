#include "archive.h"
#include "main.h"

INSTDATAC(creature, 32)

static const char place_sides[4][4] = {{1, 3, 0, 2},
{0, 1, 2, 3},
{2, 0, 3, 1},
{3, 2, 1, 0},
};
static const char* name_direction[] = {"floor",
"left", "forward", "right", "rear"};

gamei					game;
variant					party[6];
dungeon					location_above;
dungeon					location;

void gamei::setcamera(short unsigned index, direction_s direction) {
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

bool creature::ishero() const {
	return bsdata<creature>::source.indexof(this)!=-1;
}

int	gamei::getindex(const creature* p) const {
	if(!this)
		return -1;
	variant v = p;
	for(unsigned i = 0; i < sizeof(party) / sizeof(party[0]); i++) {
		if(party[i] == v)
			return i;
	}
	return -1;
}

static int find_index(int** items, int* itm) {
	for(int i = 0; i < 12; i++)
		if(items[i] == itm)
			return i;
	return -1;
}

bool gamei::question(item* current_item) {
	auto pc = getcreature(current_item);
	if(!pc || pc->gethits() <= 0)
		return false;
	char name[128]; stringbuilder sb(name); current_item->getname(sb);
	pc->say("This is %1", name);
	return true;
}

int gamei::getside(int side, direction_s dr) {
	if(dr == Center)
		return side;
	return place_sides[dr - Left][side];
}

int gamei::getsideb(int side, direction_s dr) {
	if(dr == Center)
		return side;
	for(int i = 0; i < 4; i++) {
		if(place_sides[dr - Left][i] == side)
			return i;
	}
	return -1;
}

static creature* get_best_enemy(creature** quarter, int* indecies) {
	for(int i = 0; i < 4; i++) {
		auto pc = quarter[indecies[i]];
		if(pc)
			return pc;
	}
	return 0;
}

void gamei::getheroes(creature** result, direction_s dr) {
	result[0] = result[1] = result[2] = result[3] = 0;
	for(auto v : party) {
		auto pc = v.getcreature();
		if(!pc || pc->gethits() <= 0)
			continue;
		int side = pc->getside();
		if(side > 3)
			continue;
		result[side] = pc;
	}
}

creature* gamei::getdefender(short unsigned index, direction_s dr, creature* attacker) {
	creature* defenders[4];
	int attacker_side = attacker->getside();
	if(!attacker->ishero())
		attacker_side = game.getside(attacker_side, dr);
	if(!attacker->ishero()) {
		static int sides[2][4] = {{0, 1, 2, 3}, {1, 0, 3, 2}, };
		game.getheroes(defenders, dr);
		return get_best_enemy(defenders, sides[attacker_side % 2]);
	} else {
		static int sides[2][4] = {{2, 3, 0, 1}, {3, 2, 1, 0}};
		location.getmonsters(defenders, index, dr);
		return get_best_enemy(defenders, sides[attacker_side % 2]);
	}
}

void gamei::attack(indext index_of_monsters, bool ranged) {
	creaturea parcipants;
	auto dr = getdirection();
	location.turnto(index_of_monsters, to(dr, Down));
	location.formation(index_of_monsters, to(dr, Down));
	draw::animation::update();
	parcipants.select(index_of_monsters);
	parcipants.select(game.getcamera());
	parcipants.rollinitiative();
	// All actions made in initiative order
	for(auto attacker : parcipants) {
		if(!attacker->isready())
			continue;
		attacker->attack(index_of_monsters, dr, 0, ranged);
	}
	// RULE: Hasted units make second move at end of combat round
	for(auto attacker : parcipants) {
		if(!attacker->isready())
			continue;
		if(attacker->is(Hasted)
			|| attacker->getbonus(OfSpeed, Legs)
			|| attacker->getbonus(OfSpeed, Elbow))
			attacker->attack(index_of_monsters, dr, 0, ranged);
	}
}

void read_message(dungeon* pd, dungeon::overlayi* po);

bool gamei::manipulate(item* itm, direction_s dr) {
	int index = getcamera();
	auto pc = getcreature(itm);
	auto po = location.getoverlay(index, dr);
	if(!po) {
		auto i1 = to(index, dr);
		auto cell = location.get(i1);
		switch(cell) {
		case CellPortal:
			if(pc->get(Mage))
				pc->say("This is portal");
			else
				pc->say("This is strange magical device");
			break;
		default:
			return false;
		}
		return true;
	}
	switch(location.gettype(po)) {
	case CellSecrectButton:
		creature::addexp(500, 0);
		location.set(to(index, dr), CellPassable);
		location.remove(po);
		pc->say("This is secret door");
		found_secrets++;
		break;
	case CellCellar:
		if(*itm) {
			if(bsdata<itemi>::elements[itm->gettype()].image.size == 1) {
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

static bool get_secret(short unsigned index, direction_s sight_dir, direction_s rotate_dir, direction_s& secret_dir) {
	auto po = location.getoverlay(index, to(sight_dir, rotate_dir));
	if(!po)
		return false;
	if(po->type != CellSecrectButton)
		return false;
	secret_dir = rotate_dir;
	return true;
}

void gamei::findsecrets() {
	static const char* speech[] = {
		"I see something on %1 wall",
		"There is button to the %1",
	};
	direction_s secret_dir = Center;
	auto index = game.getcamera();
	auto dir = game.getdirection();
	if(!get_secret(index, dir, Left, secret_dir)
		&& !get_secret(index, dir, Right, secret_dir))
		return;
	for(auto v : party) {
		auto pc = v.getcreature();
		if(!pc || !pc->isready())
			continue;
		if(pc->roll(DetectSecrets)) {
			pc->say(maprnd(speech), name_direction[secret_dir]);
			break;
		}
	}
}

void gamei::thrown(item* itm) {
	static char place_sides[4][2] = {{3, 1},
	{2, 3},
	{0, 2},
	{1, 0},
	};
	if(!itm || !*itm)
		return;
	auto pc = getcreature(itm);
	auto side = pc->getside() % 2;
	auto itmo = *itm;
	itm->clear();
	pc->usequick();
	auto index = draw::animation::thrown(getcamera(), getdirection(),
		itmo.gettype(), side ? Right : Left);
	location.dropitem(index, itmo, place_sides[getdirection() - Left][side]);
}

creature* gamei::getvalid(creature* pc, class_s type) const {
	auto i = pc->getpartyindex();
	if(i == -1)
		i = 0;
	auto stop = i;
	while(true) {
		auto p = party[i].getcreature();
		if(p && p->iscast(type))
			return p;
		if(++i >= (int)(sizeof(party) / sizeof(party[0])))
			i = 0;
		if(i == stop)
			return 0;
	}
}

creature* gamei::getcreature(const item* itm) const {
	for(auto v : party) {
		auto p = v.getcreature();
		if(!p)
			break;
		auto p1 = p->getitem(FirstInvertory);
		auto p2 = p->getitem(LastInvertory);
		if(itm >= p1 && itm <= p2)
			return p;
	}
	return 0;
}

bool gamei::isalive() {
	for(auto v : party) {
		auto p = v.getcreature();
		if(p && p->isready())
			return true;
	}
	return false;
}

wear_s gamei::getwear(const item* itm) const {
	auto p = getcreature(itm);
	if(p)
		return (wear_s)(FirstInvertory + (itm - p->getitem(FirstInvertory)));
	return Head;
}

void gamei::passround() {
	// Походим за монстров
	for(auto& e : location.monsters) {
		if(!e || !e.isready() || e.ismoved())
			continue;
		auto party_index = game.getcamera();
		auto party_direct = game.getdirection();
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
	for(auto v : party) {
		auto pc = v.getcreature();
		if(pc)
			pc->update(true);
	}
	// Slow update
	while(rounds_turn < rounds) {
		for(auto& e : location.monsters) {
			if(e)
				e.update_turn(false);
		}
		for(auto v : party) {
			auto pc = v.getcreature();
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
		for(auto v : party) {
			auto pc = v.getcreature();
			if(pc)
				pc->update_hour(true);
		}
		rounds_hour += 60;
		location.passhour();
	}
	location.passround();
}

void gamei::passtime(int minutes) {
	while(minutes > 0) {
		passround();
		auto count = 5;
		if(count > minutes)
			count = minutes;
		minutes -= count;
		rounds += count;
	}
}

void gamei::enter(indext index, indext level) {
	location_index = index;
	location_level = level;
	location.clear();
	location_above.clear();
	if(!location.read(location_index, location_level))
		return;
	if(location_level > 1)
		location_above.read(location_index, location_level - 1);
	draw::settiles(location.head.type);
	if(camera_index == Blocked)
		setcamera(to(location.stat.up.index, location.stat.up.dir), location.stat.up.dir);
}

bool creature::set(skill_s skill, short unsigned index) {
	variant owner = this;
	for(auto& e : location.events) {
		if(e.owner == owner
			&& e.skill == skill
			&& e.index == index)
			return false;
		if(!e.owner) {
			e.owner = owner;
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
	archive a(file, writemode);
	if(!a.signature("SAV"))
		return false;
	if(!a.version(0, 1))
		return false;
	a.set(game);
	a.set(party);
	a.set(bsdata<creature>::source);
	return true;
}

static char* fname(char* result, indext index, indext level) {
	zcpy(result, "maps/d");
	sznum(zend(result), index, 5, "00000", 10);
	sznum(zend(result), level, 2, "00", 10);
	zcat(result, ".aum");
	return result;
}

static bool serialize(dungeon& e, indext overland_index, indext level, bool write_mode) {
	char temp[260];
	io::file file(fname(temp, overland_index, level), write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write_mode);
	a.set(e);
	return true;
}

void dungeon::write() {
	serialize(*this, overland_index, level, true);
}

bool dungeon::read(indext overland_index, indext level) {
	return serialize(*this, overland_index, level, false);
}

void gamei::write() {
	if(!serialize(true))
		return;
	if(location)
		location.write();
}

bool gamei::read() {
	if(!serialize(false))
		return false;
	enter(location_index, location_level);
	return true;
}