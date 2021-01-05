#include "rect.h"
#include "main.h"

#define	gettbl(t, id) (imax(0, imin(id, (int)(sizeof(t)/sizeof(t[0])-3))))

struct roomi {
	indext			index;
	direction_s		dir;
	unsigned		flags;
};
static direction_s		all_around[] = {Left, Right, Up, Down};
static roomi			rooms[256]; // Кольцевой буфер генератора. Главное чтоб разница не была 256 значений.
static unsigned char	stack_put; // Вершина стека.
static unsigned char	stack_get; // Низ стека
typedef void fnroom(dungeon& e, direction_s dir, const sitei* site, indext* indecies);

typedef void(*dungeon_proc)(dungeon* pd, short unsigned index, direction_s dir, unsigned flags);

static void set(dungeon* pd, indext index, cell_s t1) {
	if(index != Blocked && pd->get(index) == CellUnknown)
		pd->set(index, t1);
}

static void set(dungeon* pd, indext index, direction_s dir, cell_s t1) {
	set(pd, to(index, dir), t1);
}

static void setwall(dungeon* pd, indext index, direction_s dir) {
	set(pd, index, dir, CellWall);
}

static bool isvalid(dungeon* pd, indext index, direction_s dir, cell_s t1 = CellUnknown) {
	index = to(index, dir);
	if(index == Blocked)
		return false;
	auto t = pd->get(index);
	if(t != CellUnknown && t != t1)
		return false;
	return true;
}

static bool isaround(dungeon* pd, indext index, direction_s dir, cell_s t1 = CellUnknown) {
	if(!isvalid(pd, index, to(dir, Left), t1))
		return false;
	if(!isvalid(pd, index, to(dir, Right), t1))
		return false;
	if(!isvalid(pd, index, to(dir, Up), t1))
		return false;
	return true;
}

static void putroom(dungeon* pd, indext index, direction_s dir, unsigned flags, bool test_valid) {
	if(index == Blocked)
		return;
	if(test_valid && !isvalid(pd, index, dir))
		return;
	auto& e = rooms[stack_put++];
	e.index = index;
	e.dir = dir;
	e.flags = flags;
}

static const roomi& getroom() {
	return rooms[stack_get++];
}

static bool hasrooms() {
	return stack_get != stack_put;
}

void dungeon::set(indext index, direction_s dir, cell_s type) {
	set(to(index, to(dir, Left)), CellWall);
	set(to(index, to(dir, Right)), CellWall);
	set(to(index, to(dir, Down)), CellWall);
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

static bool place(dungeon* pd, cell_s t, direction_s dir, indext index) {
	if(pd->get(index) != CellUnknown)
		return false;
	if(!isvalid(pd, index, dir))
		return false;
	pd->set(index, dir, t);
	return true;
}

static bool place(dungeon* pd, cell_s t, direction_s dir, int x, int y, int radius) {
	for(auto r = 0; r < radius; r++) {
		for(auto x1 = x - r; x1 <= x + r; x1++) {
			for(auto y1 = y - r; y1 <= y + r; y1++) {
				if(place(pd, t, dir, pd->getindex(x1, y1)))
					return true;
			}
		}
	}
	return false;
}

static direction_s random_dir() {
	return (direction_s)xrand(Left, Down);
}

static bool place(dungeon* pd, cell_s t) {
	return place(pd, t, random_dir(), xrand(2, mpx - 3), xrand(2, mpy - 3), 6);
}

static bool stairs(dungeon* pd, indext start, bool last_level) {
	if(start != Blocked)
		pd->set(start, random_dir(), CellStairsUp);
	else {
		if(!place(pd, CellStairsUp))
			return false;
	}
	if(!last_level) {
		if(!place(pd, CellStairsDown))
			return false;
	}
	return true;
}

static bool iswalls(dungeon* pd, indext index, direction_s dir) {
	if(!isvalid(pd, index, dir, CellWall))
		return false;
	auto i1 = to(index, dir);
	// Слева и справа от случайного секрета должны быть стены
	auto ni = to(i1, to(dir, Left));
	if(ni == Blocked)
		return false;
	if(pd->get(ni) != CellWall)
		return false;
	ni = to(i1, to(dir, Right));
	if(ni == Blocked)
		return false;
	if(pd->get(ni) != CellWall)
		return false;
	return true;
}

static item_s random_item_type(item_s* source, unsigned count) {
	if(!count)
		return NoItem;
	return source[rand() % count];
}

static item_s random_subtype(item_s type) {
	static item_s potions[] = {RedPotion, BluePotion, GreenPotion};
	static item_s gems[] = {GreenGem, GreenGem, GreenGem, GreenGem, BlueGem, BlueGem, BlueGem, RedGem, RedGem, PurpleGem};
	static item_s weapons[] = {AxeBattle, Club, Flail, Halberd, HammerWar, Mace, Spear, Staff, Bow, Sling};
	static item_s swords[] = {Dagger, SwordBastard, SwordLong, SwordShort, SwordTwoHanded};
	static item_s armors[] = {Robe,
		ArmorLeather, ArmorLeather, ArmorLeather, ArmorLeather,
		ArmorStuddedLeather, ArmorStuddedLeather,
		ArmorScale, ArmorScale,
		ArmorChain, ArmorBanded, ArmorPlate,
	};
	static item_s rings[] = {RedRing, BlueRing, GreenRing};
	static item_s necklages[] = {Necklage, Necklage, Necklage, Necklage, NecklageRich, NecklageRich, NecklageVeryRich};
	static item_s scrolls[] = {PriestScroll, MageScroll, MageScroll, MageScroll};
	static item_s tools[] = {HolySymbol, MagicBook, TheifTools, TheifTools, TheifTools};
	switch(type) {
	case RedPotion: return maprnd(potions);
	case RedGem: return maprnd(gems);
	case RedRing: return maprnd(rings);
	case SwordLong:
		if(d100() < 40)
			return maprnd(swords);
		return maprnd(weapons);
	case Necklage: return maprnd(necklages);
	case ArmorLeather: return maprnd(armors);
	case PriestScroll: return maprnd(scrolls);
	case TheifTools: return maprnd(tools);
	default: return type;
	}
}

static item_s random_type(bool small_size = false) {
	static item_s standart_item_types[] = {RedPotion, RedPotion, RedPotion,
		SwordLong, SwordLong, SwordLong, SwordLong, SwordLong,
		Helm, Helm,
		Shield,
		ArmorLeather, ArmorLeather, ArmorLeather,
		RedRing,
		Boots, Boots,
		Bracers,
		Necklage,
		KeyCooper,
		TheifTools,
		Arrow,
		Bones,
		DungeonMap,
		PriestScroll, PriestScroll
	};
	static item_s small_item_types[] = {RedPotion, RedPotion, RedPotion,
		RedGem,
		RedRing,
		KeyCooper, KeyCooper, KeyCooper,
		TheifTools, Arrow,
		MagicWand,
		DungeonMap,
		PriestScroll, PriestScroll, PriestScroll, PriestScroll,
	};
	item_s t = NoItem;
	if(small_size)
		t = random_item_type(small_item_types,
			sizeof(small_item_types) / sizeof(small_item_types[0]));
	else
		t = random_item_type(standart_item_types,
			sizeof(standart_item_types) / sizeof(standart_item_types[0]));
	return random_subtype(t);
}

static item create_item(dungeon* pd, item_s type, int bonus_chance_magic) {
	if(type == KeyCooper) {
		if(d100() < 60)
			type = pd->head.keys[0];
		else
			type = pd->head.keys[1];
	}
	if(type == Ration) {
		if(d100() < 30)
			type = RationIron;
	}
	item it(type);
	auto rarity = item::getrandomrarity(pd->level);
	it.setpower(rarity);
	it.setidentified(0);
	if(pd->chance.curse && (d100() < pd->chance.curse))
		it.setcursed(1);
	switch(type) {
	case Ration:
	case RationIron:
		// RULE: In dungeon 60% of all cases food will be rotten
		if(d100() < 40)
			it.setbroken(1);
		break;
	case BluePotion:
	case RedPotion:
	case GreenPotion:
		if(rarity == Common && !it.iscursed() && d100() < 60)
			it.setidentified(1);
		break;
	case RedRing:
	case BlueRing:
	case GreenRing:
		if(it.ismagical())
			pd->stat.rings++;
		break;
	case RedGem:
	case BlueGem:
	case GreenGem:
	case PurpleGem:
		pd->stat.gems++;
		break;
	case Bones:
		pd->stat.bones++;
		break;
	default:
		if(it.isartifact())
			pd->stat.artifacts++;
		break;
	}
	return it;
}

static void items(dungeon* pd, indext index, item_s type, int bonus_chance_magic) {
	pd->dropitem(index, create_item(pd, type, bonus_chance_magic), xrand(0, 3));
	pd->stat.items++;
}

static void items(dungeon* pd, indext index, int bonus_chance_magic) {
	items(pd, index, random_type(), bonus_chance_magic);
}

static void crypt_button(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	auto i1 = to(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	pd->set(i1, CellWall);
	auto po = pd->add(index, CellPuller, dir);
	po->index_link = pd->stat.crypt.index;
	pd->stat.crypt_button = *po;
}

static bool need_crypt_button(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(pd->stat.crypt_button.index == Blocked && pd->stat.crypt.index != Blocked) {
		crypt_button(pd, index, dir, flags);
		return true;
	}
	return false;
}

static void secret(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	auto i1 = to(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, to(dir, Left)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, to(dir, Right)), CellWall, CellUnknown))
		return;
	auto i2 = to(i1, dir);
	if(!isaround(pd, i2, dir, CellWall))
		return;
	pd->set(i1, CellWall);
	pd->add(index, CellSecrectButton, dir);
	pd->set(i2, CellPassable);
	int count = 1;
	if(d100() < 25)
		count = 2;
	for(int i = 0; i < count; i++)
		items(pd, i2, 5);
	pd->set(to(i2, to(dir, Left)), CellWall);
	pd->set(to(i2, to(dir, Right)), CellWall);
	pd->set(to(i2, to(dir, Up)), CellWall);
	pd->stat.secrets++;
}

static void monster(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	auto n = (d100() < 30) ? 1 : 0;
	pd->stat.monsters += pd->addmonster(pd->head.habbits[n], index);
}

static void prison(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(pd->head.type == FOREST)
		return;
	if(need_crypt_button(pd, index, dir, flags))
		return;
	auto i1 = to(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, to(dir, Left)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, to(dir, Right)), CellWall, CellUnknown))
		return;
	auto i2 = to(i1, dir);
	if(!isaround(pd, i2, dir, CellWall))
		return;
	pd->set(i1, CellDoor);
	pd->add(index, CellDoorButton, dir);
	pd->set(to(i1, to(dir, Left)), CellWall);
	pd->set(to(i1, to(dir, Right)), CellWall);
	pd->set(i2, CellPassable);
	for(int i = xrand(1, 3); i > 0; i--)
		items(pd, i2, 0);
	monster(pd, i2, Down, 0);
	pd->set(to(i2, to(dir, Left)), CellWall);
	pd->set(to(i2, to(dir, Right)), CellWall);
	pd->set(to(i2, to(dir, Up)), CellWall);
}

static void treasure(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(pd->head.type == FOREST)
		return;
	if(need_crypt_button(pd, index, dir, flags))
		return;
	auto i1 = to(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, to(dir, Left)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, to(dir, Right)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(index, to(dir, Right)), CellPassable, CellUnknown))
		return;
	auto i2 = to(i1, dir);
	if(!isaround(pd, i2, dir, CellWall))
		return;
	pd->set(i1, CellDoor);
	auto magic_bonus = 2;
	auto key_type = CellKeyHole1;
	if(d100() < 30) {
		magic_bonus += 2;
		key_type = CellKeyHole2;
	}
	pd->add(to(index, to(dir, Right)), key_type, dir);
	pd->set(to(i1, to(dir, Left)), CellWall);
	pd->set(to(i1, to(dir, Right)), CellWall);
	pd->set(i2, CellPassable);
	for(auto i = xrand(1, 3); i > 0; i--)
		items(pd, i2, magic_bonus);
	pd->set(to(i2, to(dir, Left)), CellWall);
	pd->set(to(i2, to(dir, Right)), CellWall);
	pd->set(to(i2, to(dir, Up)), CellWall);
}

static void decoration(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(pd->head.type == FOREST)
		return;
	if(need_crypt_button(pd, index, dir, flags))
		return;
	auto i1 = to(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	static cell_s random[] = {CellDecor1, CellDecor2, CellDecor3};
	pd->set(i1, CellWall);
	pd->add(index, maprnd(random), dir);
}

static void portal(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(pd->head.type == FOREST)
		return;
	if(pd->stat.portal.index != Blocked)
		return;
	auto i1 = to(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, to(dir, Left)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, to(dir, Right)), CellWall, CellUnknown))
		return;
	if(!pd->ismatch(to(i1, dir), CellWall, CellUnknown))
		return;
	pd->set(i1, to(dir, Down), CellPortal);
}

static void message(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(pd->head.type == FOREST)
		return;
	if(pd->stat.messages > MessageHabbits)
		return;
	auto i1 = to(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	pd->set(i1, CellWall);
	auto po = pd->add(index, CellMessage, dir);
	po->subtype = pd->stat.messages;
	pd->stat.messages++;
}

static bool ispassable(dungeon* pd, indext index) {
	if(index == Blocked)
		return false;
	auto t = pd->get(index);
	return t == CellPassable || t == CellUnknown;
}

static bool room(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	int i1 = to(index, dir);
	if(!isvalid(pd, index, Up, CellPassable) || !isvalid(pd, index, Right, CellPassable) || !isvalid(pd, index, Right, CellPassable)
		|| !isvalid(pd, i1, Left, CellPassable) || !isvalid(pd, i1, Right, CellPassable))
		return false;
	auto iz = index;
	auto ic = xrand(2, 4);
	short unsigned n1 = Blocked;
	short unsigned n2 = Blocked;
	for(int i = 0; i < ic; i++) {
		if(n1 == Blocked) {
			n1 = to(iz, to(dir, Left));
			if(pd->get(n1) == CellUnknown && d100() < 30)
				putroom(pd, n1, to(dir, Left), 0, true);
			else
				n1 = Blocked;
		}
		set(pd, iz, to(dir, Left), CellPassable);
		set(pd, iz, CellPassable);
		if(n2 == Blocked) {
			n2 = to(iz, to(dir, Right));
			if(pd->get(n2) == CellUnknown && d100() < 30)
				putroom(pd, n2, to(dir, Right), 0, true);
			else
				n2 = Blocked;
		}
		set(pd, iz, to(dir, Right), CellPassable);
		iz = to(iz, dir);
	}
	putroom(pd, to(iz, to(dir, Down)), dir, 0, true);
	return true;
}

static bool door(dungeon* pd, indext index, direction_s dir, bool has_button, bool has_button_on_other_side) {
	if(pd->head.type == FOREST)
		return true;
	auto i1 = to(index, dir);
	switch(pd->get(i1)) {
	case CellWall:
	case CellPortal:
		return false;
	}
	pd->set(index, CellDoor);
	if(has_button)
		pd->add(to(index, to(dir, Down)), CellDoorButton, dir);
	if(has_button_on_other_side)
		pd->add(to(index, dir), CellDoorButton, to(dir, Down));
	return true;
}

static short unsigned find_index(dungeon* pd, indext index, direction_s dir) {
	while(true) {
		auto i1 = to(index, dir);
		if(i1 == Blocked)
			return Blocked;
		switch(pd->get(i1)) {
		case CellWall:
			if(pd->getoverlay(index, dir))
				return Blocked;
			return index;
		case CellPassable:
		case CellButton:
		case CellPit:
			break;
		default:
			return Blocked;
		}
		index = i1;
	}
}

static void trap(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(pd->head.type == FOREST)
		return;
	auto dr = to(dir, Left);
	auto i1 = find_index(pd, index, dr);
	if(i1 == Blocked) {
		dr = to(dir, Right);
		i1 = find_index(pd, index, dr);
		if(i1 == Blocked)
			return;
	}
	pd->set(index, CellButton);
	auto po = pd->add(i1, CellTrapLauncher, dr);
	po->index_link = index;
	pd->stat.traps++;
}

static int random_cellar_count() {
	auto rolled = d100();
	if(rolled < 60)
		return 0;
	else if(rolled < 90)
		return 1;
	return 2;
}

static void cellar(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(pd->head.type == FOREST)
		return;
	if(need_crypt_button(pd, index, dir, flags))
		return;
	auto i1 = to(index, dir);
	if(!pd->ismatch(i1, CellWall, CellUnknown))
		return;
	pd->set(i1, CellWall);
	auto po = pd->add(index, CellCellar, dir);
	auto count = random_cellar_count();
	while(count > 0) {
		auto i1 = create_item(pd, random_type(true), 10);
		// Items in cellar can be identified
		if(d100() < 60)
			i1.setidentified(1);
		pd->add(po, i1);
		count--;
	}
}

static void empthy(dungeon* pd, indext index, direction_s dir, unsigned flags) {}

static void rations(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	items(pd, index, Ration, 0);
}

static void stones(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	items(pd, index, Stone, 0);
}

static bool test_hall(const dungeon* pd, indext& index, direction_s dir, int w, int h) {
	if(index == Blocked)
		return false;
	auto x = gx(index), y = gy(index);
	if(x + w > mpx - 2)
		x = mpx - 2 - w;
	if(y + h > mpy - 2)
		y = mpy - 2 - y;
	if(x < 1)
		x = 1;
	if(y < 1)
		y = 1;
	auto w2 = w / 2;
	for(auto x1 = x - w2; x1 < x + w2; x1++) {
		for(auto y1 = y - h; y1 < y + h; y1++) {
			auto t = pd->get(pd->get(x1, y1));
			if(t != CellUnknown)
				return false;
		}
	}
	return true;
}

static void set(dungeon* pd, indext index, int w, int h, cell_s v) {
	if(index == Blocked)
		return;
	auto x = gx(index), y = gy(index);
	auto w2 = w / 2;
	for(auto x1 = x - w2; x1 < x + w2; x1++)
		for(auto y1 = y - h; y1 < y + h; y1++)
			pd->set(pd->get(x1, y1), v);
}

static void corridor(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	auto chance = 0;
	if(index == Blocked)
		return;
	direction_s rnd[] = {Right, Left};
	if(d100() < 50)
		iswap(rnd[0], rnd[1]);
	short unsigned start = Blocked;
	while(true) {
		int new_index = to(index, dir);
		if(new_index == Blocked || pd->get(new_index) != CellUnknown)
			break;
		bool random_content = true;
		if(start == Blocked) {
			start = index;
			if(flags&EmpthyStartIndex)
				random_content = false;
		}
		index = new_index;
		pd->set(index, CellPassable);
		if(d100() < chance || to(index, dir) == Blocked) {
			if(d100() < 20 && pd->stat.elements > 10) {
				if(room(pd, index, dir, flags))
					return;
			}
			break;
		}
		setwall(pd, index, to(dir, Left));
		setwall(pd, index, to(dir, Right));
		if(random_content && (chance == 0) && d100() < 30) {
			if(door(pd, index, dir, true, true))
				random_content = false;
		}
		if(random_content) {
			static dungeon_proc corridor_random[] = {empthy,
				empthy, empthy, empthy, empthy, empthy, empthy,
				empthy, empthy, empthy, empthy,
				secret,
				monster, monster, monster, monster,
				rations,
				stones,
				trap,
				cellar,
				portal,
				prison, prison,
				treasure,
				decoration, decoration,
				message,
			};
			auto proc = corridor_random[rand() % (sizeof(corridor_random) / sizeof(corridor_random[0]))];
			proc(pd, index, to(dir, rnd[0]), flags);
			if(d100() < 60)
				iswap(rnd[0], rnd[1]);
		}
		chance += 13;
	}
	if(start == Blocked)
		return;
	auto passes = 0;
	if(ispassable(pd, to(index, to(dir, rnd[0])))) {
		passes++;
		putroom(pd, index, to(dir, rnd[0]), 0, false);
	}
	if(ispassable(pd, to(index, to(dir, rnd[1])))) {
		passes++;
		putroom(pd, index, to(dir, rnd[1]), 0, false);
	}
	if(ispassable(pd, to(index, dir))) {
		if(passes < 1)
			putroom(pd, index, dir, 0, false);
	}
}

static bool is_valid_dungeon(dungeon* pd) {
	unsigned short pathmap[mpy*mpx];
	if(pd->stat.down.index == Blocked || pd->stat.up.index == Blocked)
		return true;
	pd->getblocked(pathmap, true);
	pd->makewave(pd->stat.up.index, pathmap);
	return pathmap[pd->stat.down.index] != 0;
}

static void remove_all_overlay(dungeon* pd, indext index) {
	if(index == Blocked)
		return;
	for(auto& e : pd->overlays) {
		if(!e.type)
			break;
		if(e.dir == Center)
			continue;
		if(to(e.index, e.dir) == index)
			e.index = 0;
	}
}

static void drop_special_ground(item& it, indext index) {
	location.dropitem(index, it, 0);
	location.stat.wands.index = index;
	location.stat.wands.dir = Center;
	it.clear();
}

static void validate_special_items(dungeon& location) {
	if(!location.head.wands)
		return;
	if(location.stat.wands)
		return;
	item it(location.head.wands);
	if(d100() < 50)
		it.setpower(VeryRare);
	if(d100() < 10)
		it.setcursed(1);
	if(it && it.issmall()) {
		adat<dungeon::overlayi*, 512> source;
		for(auto& e : location.overlays) {
			if(!e || e.type != CellCellar)
				continue;
			source.add(&e);
		}
		if(source) {
			auto p = source.data[rand() % source.count];
			location.add(p, it);
			location.stat.wands = *p;
			it.clear();
		}
	}
	if(it) {
		adat<indext, 512> source;
		for(auto& e : location.items) {
			if(!e || (e.gettype() != Ration && e.gettype() != RationIron))
				continue;
			source.add(e.index);
		}
		if(source) {
			auto i = source.data[rand() % source.count];
			drop_special_ground(it, i);
		}
	}
	if(it && location.stat.portal)
		drop_special_ground(it, location.stat.portal.index);
	if(it && location.stat.down)
		drop_special_ground(it, location.stat.down.index);
	if(it && location.stat.up)
		drop_special_ground(it, location.stat.up.index);
}

static void remove_dead_door(dungeon* pd) {
	for(short unsigned i = 0; i < mpx*mpy; i++) {
		auto t = pd->get(i);
		if(t != CellDoor)
			continue;
		if(pd->get(to(i, Left)) == CellWall && pd->get(to(i, Right)) == CellWall)
			continue;
		if(pd->get(to(i, Up)) == CellWall && pd->get(to(i, Down)) == CellWall)
			continue;
		pd->set(i, CellPassable);
		remove_all_overlay(pd, i);
	}
}

static void link_dungeon(dungeon& location, dungeon& below) {
	unsigned short pm1[mpy*mpx];
	unsigned short pm2[mpy*mpx];
	unsigned short pme[mpx*mpy];
	// 1) Get idicies of two linked dungeons
	location.getblocked(pm1, true);
	location.makewave(to(location.stat.down.index, location.stat.down.dir), pm1);
	below.getblocked(pm2, true);
	below.makewave(to(below.stat.down.index, below.stat.down.dir), pm2);
	// 2) Get valid indicies
	for(int i = 1; i < mpx*mpy; i++) {
		// Second dungeon must be passable
		if(!pm2[i] || pm2[i] == Blocked)
			pm1[i] = Blocked;
		// Second dungeon must not have door in this cell (door cell is passable)
		if(below.get(i) == CellDoor || location.get(i) == CellDoor)
			pm1[i] = Blocked;
		// There is no location right before stairs
		if(i == to(below.stat.down.index, below.stat.down.dir)
			|| i == to(below.stat.up.index, below.stat.up.dir)
			|| i == to(location.stat.up.index, location.stat.up.dir)
			|| i == to(location.stat.down.index, location.stat.down.dir))
			pm1[i] = Blocked;
	}
	// 3) Get possible pits indicies
	auto p = pme;
	for(int i = 1; i < mpx*mpy; i++) {
		if(pm1[i] && pm1[i] != Blocked)
			*p++ = i;
	}
	auto place_count = p - pme;
	// 4) Place random count of pits
	auto pits_count = xrand(1, 4);
	zshuffle(pme, place_count);
	if(pits_count > place_count)
		pits_count = place_count;
	for(int i = 0; i < pits_count; i++)
		location.set(pme[i], CellPit);
}

static bool isroom(const dungeon& location, int x, int y, int r) {
	for(auto x1 = x - r; x1 <= x + r; x1++) {
		for(auto y1 = y - r; y1 <= y + r; y1++) {
			auto i = location.getindex(x, y);
			if(i == Blocked)
				return false;
			if(location.isblocked(i))
				return false;
		}
	}
	return true;
}

static unsigned find_rooms(indext* source, const indext* pe, const dungeon& location) {
	auto pb = source;
	for(auto x = 0; x < mpx; x++) {
		for(auto y = 0; y < mpy; y++) {
			if(isroom(location, x, y, 1)) {
				if(pb < pe) {
					auto i = location.getindex(x, y);
					if(i != Blocked)
						*pb++ = i;
				}
			}
		}
	}
	return pb - source;
}

static void add_spawn_points(dungeon& location) {
	adat<short unsigned> source;
	source.count = find_rooms(source.data, source.endof(), location);
	if(!source.count)
		return;
	zshuffle(source.data, source.count);
	for(unsigned i = 0; i < sizeof(location.stat.spawn) / sizeof(location.stat.spawn[0]); i++)
		location.stat.spawn[i] = source.data[i];
}

static void den(dungeon* pd, indext index, direction_s dir, unsigned flags) {
	if(!pd->isroom(index, dir, 2, 6))
		return;
}

static void stairs_up(dungeon& e, direction_s dir, const sitei* p, indext* indecies) {
	e.set(indecies[0], dir, CellStairsUp);
}

static void stairs_down(dungeon& e, direction_s dir, const sitei* p, indext* indecies) {
	e.set(indecies[0], dir, CellStairsDown);
}

static void empthy_room(dungeon& e, direction_s dir, const sitei* p, indext* indecies) {
}

static void create_crypt(dungeon& e, direction_s dir, const sitei* p, indext* indecies) {
	e.stat.crypt;
	e.stat.crypt.index = indecies[0];
	e.stat.crypt.dir = dir;
	e.set(e.stat.crypt.index, CellDoor);
	e.stat.monsters += e.addmonster(p->crypt.boss, indecies[1]);
	putroom(&e, e.stat.crypt.index, e.stat.crypt.dir, EmpthyStartIndex, false);
}

static void create_room(dungeon& e, indext index, shape_s place, direction_s dir, bool mirror, const sitei* site, fnroom proc, bool place_in_zero_index) {
	if(index == Blocked)
		return;
	indext indecies[10];
	point size;
	e.set(index, dir, place, size, indecies, true, d100()<50, place_in_zero_index);
	proc(e, dir, site, indecies);
	putroom(&e, indecies[0], dir, EmpthyStartIndex, false);
}

static void create_room(dungeon& e, shape_s place, const sitei* site, fnroom proc) {
	indext indecies[10]; point size;
	auto mirror = d100() < 50;
	auto dir = maprnd(all_around);
	e.set(0, dir, place, size, indecies, false, mirror);
	short x = xrand(3, mpx - 3 - size.x), y = xrand(3, mpy - 3 - size.y);
	auto i = e.getvalid(e.getindex(x, y), size.x, size.y, CellUnknown);
	create_room(e, i, place, dir, mirror, site, proc, false);
}

static shape_s random(shape_s v1, shape_s v2) {
	return (d100() < 50) ? v1 : v2;
}

void adventurei::create(bool interactive) const {
	auto count = levels->getleveltotal();
	if(!count)
		return;
	auto dungeons = new dungeon[count];
	if(!dungeons)
		return;
	auto base = 0;
	dungeon* previous = 0;
	for(auto p = levels; *p; p++) {
		auto special_item_level = -1;
		if(p->head.wands)
			special_item_level = rand() % p->levels;
		for(auto j = 0; j < p->levels; j++) {
			auto& e = dungeons[base + j];
			auto level = base + j + 1;
			auto start = Blocked;
			if(previous)
				start = previous->stat.down.index;
			auto last_level = (level == count);
			while(true) {
				e.clear();
				e.overland_index = bsdata<adventurei>::source.indexof(this);
				e.head = p->head;
				if(special_item_level != j)
					e.head.wands = NoItem;
				e.level = level;
				e.chance.curse = 5 + p->chance.curse;
				if(start == Blocked)
					create_room(e, ShapeDeadEnd, p, stairs_up);
				else
					create_room(e, start, ShapeDeadEnd, maprnd(all_around), false, 0, stairs_up, true);
				if(!last_level)
					create_room(e, ShapeDeadEnd, p, stairs_down);
				if(p->crypt.boss)
					create_room(e, ShapeRoom, p, create_crypt);
				while(hasrooms()) {
					auto ev = getroom();
					corridor(&e, ev.index, ev.dir, ev.flags);
					e.stat.elements++;
					if(interactive)
						e.automap(false);
				}
				e.finish(CellWall);
				if(is_valid_dungeon(&e))
					break;
			}
			remove_dead_door(&e);
			if(j==special_item_level)
				validate_special_items(e);
			add_spawn_points(e);
			e.overland_index = bsdata<adventurei>::source.indexof(this);
			previous = &e;
		}
		base += p->levels;
	}
	for(unsigned i = 0; i < count - 1; i++)
		link_dungeon(dungeons[i], dungeons[i + 1]);
	for(unsigned i = 0; i < count; i++)
		dungeons[i].write();
	delete[] dungeons;
}