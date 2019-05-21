#include "rect.h"
#include "main.h"

#define	gettbl(t, id) (imax(0, imin(id, (int)(sizeof(t)/sizeof(t[0])-3))))

enum flagtype {
	EmpthyStartIndex = 1,
};

struct roominfo {
	short unsigned	index;
	direction_s		dir;
	unsigned		flags;
};
static roominfo		rooms[256]; // ��������� ����� ����������. ������� ����� ������� �� ���� 256 ��������.
static unsigned char stack_put; // ������� �����.
static unsigned char stack_get; // ��� �����.

static void set(dungeon* pd, short unsigned index, cell_s t1) {
	if(index && pd->get(index) == CellUnknown)
		pd->set(index, t1);
}

static void set(dungeon* pd, short unsigned index, direction_s dir, cell_s t1) {
	set(pd, moveto(index, dir), t1);
}

static void setwall(dungeon* pd, short unsigned index, direction_s dir) {
	set(pd, index, dir, CellWall);
}

static bool isvalid(dungeon* pd, short unsigned index, direction_s dir, cell_s t1 = CellUnknown) {
	index = moveto(index, dir);
	if(!index)
		return false;
	auto t = pd->get(index);
	if(t != CellUnknown && t != t1)
		return false;
	return true;
}

static bool isaround(dungeon* pd, short unsigned index, direction_s dir, cell_s t1 = CellUnknown) {
	if(!isvalid(pd, index, rotateto(dir, Left), t1))
		return false;
	if(!isvalid(pd, index, rotateto(dir, Right), t1))
		return false;
	if(!isvalid(pd, index, rotateto(dir, Up), t1))
		return false;
	return true;
}

static void putroom(dungeon* pd, unsigned short index, direction_s dir, unsigned flags, bool test_valid) {
	if(!index)
		return;
	if(test_valid && !isvalid(pd, index, dir))
		return;
	auto& e = rooms[stack_put++];
	e.index = index;
	e.dir = dir;
	e.flags = flags;
}

static const roominfo& getroom() {
	return rooms[stack_get++];
}

static bool hasrooms() {
	return stack_get != stack_put;
}

void dungeon::setelement(short unsigned index, direction_s dir, cell_s type) {
	set(moveto(index, rotateto(dir, Left)), CellWall);
	set(moveto(index, rotateto(dir, Right)), CellWall);
	set(moveto(index, rotateto(dir, Down)), CellWall);
	set(index, type);
	switch(type) {
	case CellStairsUp:
		stat.up.index = index;
		stat.up.dir = dir;
		break;
	case CellStairsDown:
		stat.down.index = index;
		stat.down.dir = dir;
		break;
	case CellPortal:
		stat.portal.index = index;
		stat.portal.dir = dir;
		break;
	default:
		break;
	}
}

static bool place(dungeon* pd, cell_s t, direction_s dir) {
	auto x = xrand(0, mpx - 1);
	auto y = xrand(0, mpy - 1);
	if(x == 0 && y == 0)
		return false;
	if(dir == Left && x <= 0)
		dir = Right;
	if(dir == Right && x >= mpx - 1)
		dir = Left;
	if(dir == Up && y <= 0)
		dir = Down;
	if(dir == Down && y >= mpy - 1)
		dir = Up;
	auto i = pd->getindex(x, y);
	if(pd->get(i) != CellUnknown)
		return false;
	if(!isvalid(pd, i, dir))
		return false;
	pd->setelement(i, dir, t);
	return true;
}

static void stairs(dungeon* pd, unsigned short start) {
	if(start)
		pd->setelement(start, (direction_s)xrand(Left, Down), CellStairsUp);
	else
		while(!place(pd, CellStairsUp, (direction_s)xrand(Left, Down)));
	while(!place(pd, CellStairsDown, (direction_s)xrand(Left, Down)));
}

static bool iswalls(dungeon* pd, short unsigned index, direction_s dir) {
	if(!isvalid(pd, index, dir, CellWall))
		return false;
	auto i1 = moveto(index, dir);
	// ����� � ������ �� ���������� ������� ������ ���� �����
	auto ni = moveto(i1, rotateto(dir, Left));
	if(!ni)
		return false;
	if(pd->get(ni) != CellWall)
		return false;
	ni = moveto(i1, rotateto(dir, Right));
	if(!ni)
		return false;
	if(pd->get(ni) != CellWall)
		return false;
	return true;
}

static item_s random_type(bool small_item = false) {
	struct chance_info {
		int		chance;
		item_s	type;
	};
	static chance_info elements[] = {{1, PotionRed},
	{1, PotionBlue},
	{1, PotionGreen},
	{1, RedGem},
	{1, BlueGem},
	{1, GreenGem},
	{1, PurpleGem},
	{4, SwordLong},
	{2, Helm},
	{2, ArmorLeather},
	{1, BlueRing},
	{1, RedRing},
	{1, GreenRing},
	{2, Boots},
	{2, Bracers},
	{2, KeyCooper},
	{2, TheifTools},
	{1, MagicBook},
	{1, HolySymbol},
	{1, Bones},
	{2, MagicWand},
	{2, DungeonMap},
	{4, PriestScroll},
	{4, MageScroll},
	};
	static chance_info small_elements[] = {{1, PotionRed},
	{1, PotionBlue},
	{1, PotionGreen},
	{1, RedGem},
	{1, BlueGem},
	{1, GreenGem},
	{1, PurpleGem},
	{1, BlueRing},
	{1, RedRing},
	{1, GreenRing},
	{2, Bracers},
	{2, KeyCooper},
	{2, TheifTools},
	{1, MagicBook},
	{1, HolySymbol},
	{1, Bones},
	{2, MagicWand},
	{2, DungeonMap},
	{4, PriestScroll},
	{4, MageScroll},
	};
	auto total = 0;
	if(small_item) {
		for(auto& e : small_elements)
			total += e.chance;
		if(total) {
			int d = rand() % total;
			for(auto& e : small_elements) {
				if(d < e.chance)
					return e.type;
				d -= e.chance;
			}
		}
	} else {
		for(auto& e : elements)
			total += e.chance;
		if(total) {
			int d = rand() % total;
			for(auto& e : elements) {
				if(d < e.chance)
					return e.type;
				d -= e.chance;
			}
		}
	}
	return NoItem;
}

static item create_item(dungeon* pd, item_s type, int bonus_chance_magic) {
	if(type == KeyCooper) {
		if(d100() < 60)
			type = pd->stat.keys[0];
		else
			type = pd->stat.keys[1];
	}
	if(type == Ration) {
		if(d100() < 30)
			type = RationIron;
	}
	int chance_magic = imax(0, imin(75, 12 + pd->level * 3) + bonus_chance_magic);
	item it(type, chance_magic);
	it.setidentified(0);
	switch(type) {
	case Ration:
	case RationIron:
		// RULE: In dungeon 60% of all cases food will be rotten
		if(d100() < 60)
			it.setbroken(1);
		break;
	case RedRing:
	case BlueRing:
	case GreenRing:
		if(it.ismagical())
			pd->stat.rings++;
		break;
	default:
		if(it.getmagic() >= 4)
			pd->stat.artifacts++;
		break;
	}
	return it;
}

static void items(dungeon* pd, short unsigned index, item_s type, int bonus_chance_magic) {
	pd->dropitem(index, create_item(pd, type, bonus_chance_magic), xrand(0, 3));
}

static void items(dungeon* pd, short unsigned index, int bonus_chance_magic) {
	pd->dropitem(index, create_item(pd, random_type(), bonus_chance_magic), xrand(0, 3));
}

static void secret(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	auto i1 = moveto(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, rotateto(dir, Left)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, rotateto(dir, Right)), CellWall, CellUnknown))
		return;
	auto i2 = moveto(i1, dir);
	if(!isaround(pd, i2, dir, CellWall))
		return;
	pd->set(i1, CellWall);
	pd->setoverlay(index, CellSecrectButton, dir);
	pd->set(i2, CellPassable);
	int count = xrand(1, 4);
	for(int i = 0; i < count; i++)
		items(pd, i2, 20);
	pd->set(moveto(i2, rotateto(dir, Left)), CellWall);
	pd->set(moveto(i2, rotateto(dir, Right)), CellWall);
	pd->set(moveto(i2, rotateto(dir, Up)), CellWall);
	pd->stat.secrets++;
}

static void monster(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	int n = 0;
	int d = d100();
	if(d < 30)
		n = 1;
	pd->addmonster(pd->stat.habbits[n], index);
}

static void prison(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	auto i1 = moveto(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, rotateto(dir, Left)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, rotateto(dir, Right)), CellWall, CellUnknown))
		return;
	auto i2 = moveto(i1, dir);
	if(!isaround(pd, i2, dir, CellWall))
		return;
	pd->set(i1, CellDoor);
	pd->setoverlay(index, CellDoorButton, dir);
	pd->set(moveto(i1, rotateto(dir, Left)), CellWall);
	pd->set(moveto(i1, rotateto(dir, Right)), CellWall);
	pd->set(i2, CellPassable);
	for(int i = xrand(1, 3); i > 0; i--)
		items(pd, i2, 0);
	monster(pd, i2, Down, 0);
	pd->set(moveto(i2, rotateto(dir, Left)), CellWall);
	pd->set(moveto(i2, rotateto(dir, Right)), CellWall);
	pd->set(moveto(i2, rotateto(dir, Up)), CellWall);
}

static void treasure(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	auto i1 = moveto(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, rotateto(dir, Left)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, rotateto(dir, Right)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(index, rotateto(dir, Right)), CellPassable, CellUnknown))
		return;
	auto i2 = moveto(i1, dir);
	if(!isaround(pd, i2, dir, CellWall))
		return;
	pd->set(i1, CellDoor);
	auto magic_bonus = 10;
	auto key_type = CellKeyHole1;
	if(d100() < 30) {
		magic_bonus += 5;
		key_type = CellKeyHole2;
	}
	pd->setoverlay(moveto(index, rotateto(dir, Right)), key_type, dir);
	pd->set(moveto(i1, rotateto(dir, Left)), CellWall);
	pd->set(moveto(i1, rotateto(dir, Right)), CellWall);
	pd->set(i2, CellPassable);
	for(int i = xrand(1, 3); i > 0; i--)
		items(pd, i2, magic_bonus);
	pd->set(moveto(i2, rotateto(dir, Left)), CellWall);
	pd->set(moveto(i2, rotateto(dir, Right)), CellWall);
	pd->set(moveto(i2, rotateto(dir, Up)), CellWall);
}

static void decoration(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	auto i1 = moveto(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	static cell_s random[] = {CellDecor1, CellDecor2, CellDecor3};
	pd->set(i1, CellWall);
	pd->setoverlay(index, maprnd(random), dir);
}

static void portal(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	if(pd->stat.portal.index)
		return;
	auto i1 = moveto(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, rotateto(dir, Left)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, rotateto(dir, Right)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(moveto(i1, dir), CellWall, CellUnknown))
		return;
	pd->setelement(i1, rotateto(dir, Down), CellPortal);
}

static void message(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	if(pd->stat.messages > MessageAtifacts)
		return;
	auto i1 = moveto(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	pd->set(i1, CellWall);
	auto po = pd->setoverlay(index, CellMessage, dir);
	po->subtype = pd->stat.messages;
	pd->stat.messages++;
}

static bool ispassable(dungeon* pd, short unsigned index) {
	if(!index)
		return false;
	auto t = pd->get(index);
	return t == CellPassable || t == CellUnknown;
}

static bool room(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	int i1 = moveto(index, dir);
	if(!isvalid(pd, index, Up, CellPassable) || !isvalid(pd, index, Right, CellPassable) || !isvalid(pd, index, Right, CellPassable)
		|| !isvalid(pd, i1, Left, CellPassable) || !isvalid(pd, i1, Right, CellPassable))
		return false;
	auto iz = index;
	auto ic = xrand(2, 4);
	short unsigned n1 = 0;
	short unsigned n2 = 0;
	for(int i = 0; i < ic; i++) {
		if(!n1) {
			n1 = moveto(iz, rotateto(dir, Left));
			if(pd->get(n1) == CellUnknown && d100() < 30)
				putroom(pd, n1, rotateto(dir, Left), 0, true);
			else
				n1 = 0;
		}
		set(pd, iz, rotateto(dir, Left), CellPassable);
		set(pd, iz, CellPassable);
		if(!n2) {
			n2 = moveto(iz, rotateto(dir, Right));
			if(pd->get(n2) == CellUnknown && d100() < 30)
				putroom(pd, n2, rotateto(dir, Right), 0, true);
			else
				n2 = 0;
		}
		set(pd, iz, rotateto(dir, Right), CellPassable);
		iz = moveto(iz, dir);
	}
	putroom(pd, moveto(iz, rotateto(dir, Down)), dir, 0, true);
	return true;
}

static bool door(dungeon* pd, short unsigned index, direction_s dir, bool has_button, bool has_button_on_other_side) {
	auto i1 = moveto(index, dir);
	switch(pd->get(i1)) {
	case CellWall:
	case CellPortal:
		return false;
	}
	pd->set(index, CellDoor);
	if(has_button)
		pd->setoverlay(moveto(index, rotateto(dir, Down)), CellDoorButton, dir);
	if(has_button_on_other_side)
		pd->setoverlay(moveto(index, dir), CellDoorButton, rotateto(dir, Down));
	return true;
}

static short unsigned find_index(dungeon* pd, short unsigned index, direction_s dir) {
	while(true) {
		auto i1 = moveto(index, dir);
		if(!i1)
			return 0;
		switch(pd->get(i1)) {
		case CellWall:
			if(pd->getoverlay(index, dir))
				return 0;
			return index;
		case CellPassable:
		case CellButton:
		case CellPit:
			break;
		default:
			return 0;
		}
		index = i1;
	}
}

static void trap(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	dir = rotateto(dir, Down);
	auto i1 = find_index(pd, index, dir);
	if(!i1)
		return;
	pd->set(index, CellButton);
	auto po = pd->setoverlay(i1, CellTrapLauncher, dir);
	pd->stat.traps++;
}

static void cellar(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	auto i1 = moveto(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	pd->set(i1, CellWall);
	auto po = pd->setoverlay(index, CellCellar, dir);
	auto count = 0;
	if(d100() < 50)
		count = xrand(1, 3);
	while(count-->0)
		pd->add(po, create_item(pd, random_type(true), 10));
}

static void empthy(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {}

static void rations(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	items(pd, index, Ration, 0);
}

static void corridor(dungeon* pd, short unsigned index, direction_s dir, unsigned flags) {
	auto chance = 0;
	if(!index)
		return;
	direction_s rnd[] = {Right, Left};
	if(d100() < 50)
		iswap(rnd[0], rnd[1]);
	short unsigned start = 0;
	while(true) {
		int new_index = moveto(index, dir);
		if(!new_index || pd->get(new_index) != CellUnknown)
			break;
		bool random_content = true;
		if(!start) {
			start = index;
			if(flags&EmpthyStartIndex)
				random_content = false;
		}
		index = new_index;
		pd->set(index, CellPassable);
		if(d100() < chance || !moveto(index, dir)) {
			if(d100() < 20 && pd->stat.elements > 10) {
				if(room(pd, index, dir, flags))
					return;
			}
			break;
		}
		setwall(pd, index, rotateto(dir, Left));
		setwall(pd, index, rotateto(dir, Right));
		if(random_content && (chance == 0) && d100() < 30) {
			if(door(pd, index, dir, true, true))
				random_content = false;
		}
		if(random_content) {
			static struct chance_info {
				int chance;
				void(*proc)(dungeon* pd, short unsigned index, direction_s dir, unsigned flags);
				bool swap;
			} corridor_random[] = {{13, empthy, false},
			{1, secret, true},
			{4, monster, false},
			{1, rations, false},
			{1, trap, false},
			{2, cellar, true},
			{1, portal, true},
			{2, prison, true},
			{1, treasure, true},
			{2, decoration, true},
			{1, message, true}
			};
			auto total = 0;
			for(auto& e : corridor_random)
				total += e.chance;
			if(total) {
				int d = rand() % total;
				for(auto& e : corridor_random) {
					if(d < e.chance) {
						if(e.swap) {
							e.proc(pd, index, rotateto(dir, rnd[0]), flags);
							if(d100() < 60)
								iswap(rnd[0], rnd[1]);
						} else
							e.proc(pd, index, dir, flags);
						break;
					}
					d -= e.chance;
				}
			}
		}
		chance += 13;
	}
	if(start) {
		int passes = 0;
		if(ispassable(pd, moveto(index, rotateto(dir, rnd[0])))) {
			passes++;
			putroom(pd, index, rotateto(dir, rnd[0]), 0, false);
		}
		if(ispassable(pd, moveto(index, rotateto(dir, rnd[1])))) {
			passes++;
			putroom(pd, index, rotateto(dir, rnd[1]), 0, false);
		}
		if(ispassable(pd, moveto(index, dir))) {
			if(passes < 1)
				putroom(pd, index, dir, 0, false);
		}
	}
}

static bool is_valid_dungeon(dungeon* pd) {
	unsigned short pathmap[mpy*mpx];
	if(!pd->stat.down.index || !pd->stat.up.index)
		return false;
	pd->getblocked(pathmap, true);
	pd->makewave(pd->stat.up.index, pathmap);
	return pathmap[pd->stat.down.index] != 0;
}

static void remove_all_overlay(dungeon* pd, short unsigned index) {
	if(!index)
		return;
	for(auto& e : pd->overlays) {
		if(!e.type)
			break;
		if(e.dir == Center)
			continue;
		if(moveto(e.index, e.dir) == index)
			e.index = 0;
	}
}

static void remove_dead_door(dungeon* pd) {
	for(short unsigned i = 1; i < mpx*mpy; i++) {
		auto t = pd->get(i);
		if(t != CellDoor)
			continue;
		if(pd->get(moveto(i, Left)) == CellWall && pd->get(moveto(i, Right)) == CellWall)
			continue;
		if(pd->get(moveto(i, Up)) == CellWall && pd->get(moveto(i, Down)) == CellWall)
			continue;
		pd->set(i, CellPassable);
		remove_all_overlay(pd, i);
	}
}

void dungeon::generate(dungeon_s type, unsigned short index, unsigned char level, unsigned short start, bool interactive) {
	while(true) {
		clear();
		overland_index = index;
		setcontent(type, level);
		stairs(this, start);
		putroom(this, stat.up.index, stat.up.dir, EmpthyStartIndex, false);
		putroom(this, stat.down.index, stat.down.dir, EmpthyStartIndex, false);
		while(hasrooms()) {
			auto e = getroom();
			corridor(this, e.index, e.dir, e.flags);
			stat.elements++;
			if(interactive)
				game::action::automap(*this, false);
		}
		finish(CellWall);
		if(is_valid_dungeon(this))
			break;
	}
	remove_dead_door(this);
	if(overland_index)
		write();
}

void dungeon::link() {
	if(haspits)
		return;
	dungeon below;
	if(!below.read(overland_index, level + 1))
		below.generate(type, overland_index, level + 1, stat.down.index);
	unsigned short pm1[mpy*mpx];
	unsigned short pm2[mpy*mpx];
	unsigned short elements[32 * 32];
	// 1) Get idicies of two linked dungeons
	getblocked(pm1, true);
	makewave(moveto(stat.down.index, stat.down.dir), pm1);
	below.getblocked(pm2, true);
	below.makewave(moveto(below.stat.down.index, below.stat.down.dir), pm2);
	// 2) Get valid indicies
	for(int i = 1; i < mpx*mpy; i++) {
		// Second dungeon must be passable
		if(!pm2[i] || pm2[i] == Blocked)
			pm1[i] = Blocked;
		// Second dungeon must not have door in this cell (door cell is passable)
		if(below.get(i) == CellDoor || get(i) == CellDoor)
			pm1[i] = Blocked;
		// There is no location right before stairs
		if(i == moveto(below.stat.down.index, below.stat.down.dir)
			|| i == moveto(below.stat.up.index, below.stat.up.dir)
			|| i == moveto(stat.up.index, stat.up.dir)
			|| i == moveto(stat.down.index, stat.down.dir))
			pm1[i] = Blocked;
	}
	// 3) Get possible pits indicies
	auto p = elements;
	for(int i = 1; i < mpx*mpy; i++) {
		if(pm1[i] && pm1[i] != Blocked)
			*p++ = i;
	}
	*p = 0;
	// 4) Place random count of pits
	auto pits_count = xrand(1, 4);
	auto count = zlen(elements);
	zshuffle(elements, count);
	if(pits_count > count)
		pits_count = count;
	for(int i = 0; i < pits_count; i++)
		set(elements[i], CellPit);
	haspits = true;
	write();
}