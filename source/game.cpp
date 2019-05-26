#include "archive.h"
#include "main.h"

const int				chance_broke_instrument = 13;
static unsigned short	camera_index;
static direction_s		camera_direction;
static unsigned			overland_index = 1;
static unsigned char	location_level = 1;
creature*				game::party[7];
unsigned				game::rounds;
dungeon					location_above;
dungeon					location;
static creature			hero_data[32];

static struct event_info {
	unsigned			rounds;
	const creature*		pc;
	skill_s				skill;
	short unsigned		index;
	operator bool() const { return pc != 0; }
} events[256];

static const char place_sides[4][4] = {{1, 3, 0, 2},
{0, 1, 2, 3},
{2, 0, 3, 1},
{3, 2, 1, 0},
};
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
	if(!index)
		return;
	int x = gx(index);
	int y = gy(index);
	location.set(index, CellExplored);
	location.set(location.getindex(x - 1, y), CellExplored);
	location.set(location.getindex(x + 1, y), CellExplored);
	location.set(location.getindex(x, y + 1), CellExplored);
	location.set(location.getindex(x, y - 1), CellExplored);
}

size_s game::getsize(item_s id) {
	switch(id) {
	case AxeBattle:
	case Axe:
	case Bow:
	case Flail:
	case Halberd:
	case Mace:
	case Robe:
	case Shield:
	case SwordLong:
	case SwordShort:
	case SwordBastard:
	case SwordTwoHanded:
	case Spear:
	case Staff:
	case HammerWar:
		return Large;
	default:
		if(id >= ArmorLeather && id <= ArmorPlate)
			return Large;
		return Medium;
	}
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

static int compare_int(const void* p1, const void* p2) {
	return *((int*)p2) - *((int*)p1);
}

void game::getability(int* result, class_s type, race_s race) {
	for(int i = 0; i < 6; i++)
		result[i] = xrand(1, 6) + xrand(1, 6) + xrand(1, 6);
	qsort(result, 6, sizeof(result[0]), compare_int);
	zshuffle(result, 6);
	int max_position = -1;
	int max_value = 0;
	for(int i = 0; i < 6; i++) {
		if(result[i] > max_value) {
			max_position = i;
			max_value = result[i];
		}
	}
	auto bst_position = bsmeta<classi>::elements[type].ability;
	if(max_position != -1)
		iswap(result[max_position], result[bst_position]);
	// Check maximum by class
	for(int j = 0; j < 6; j++) {
		int m = bsmeta<classi>::elements[type].minimum[j];
		if(result[j] < m)
			result[j] = m;
	}
	// Check minimum by race
	for(auto j = 0; j < 6; j++) {
		auto m = bsmeta<racei>::elements[race].minimum[j];
		if(result[j] < m)
			result[j] = m;
	}
	// Check maximum by race
	for(auto j = 0; j < 6; j++) {
		auto m = bsmeta<racei>::elements[race].maximum[j];
		if(result[j] > m)
			result[j] = m;
	}
	// Adjust ability
	for(auto j = 0; j < 6; j++)
		result[j] += bsmeta<racei>::elements[race].adjustment[j];
}

int game::getfreeside(creature* sides[4]) {
	if(sides[0] && sides[0]->getsize() >= Large)
		return -1;
	for(int i = 0; i < 4; i++) {
		if(!sides[i])
			return i;
	}
	return -1;
}

command_s game::action::move(direction_s direction) {
	int i = getcamera();
	int i1 = moveto(i, vectorized(getdirection(), direction));
	int t = location.get(i1);
	if(location.isblocked(i1) || location.ismonster(i1)
		|| ((t == CellStairsUp || t == CellStairsDown) && direction != Up)) {
		mslog("You can\'t go that way");
		return NoCommand;
	}
	switch(t) {
	case CellStairsUp:
		mslog("Going up");
		return GoingUp;
	case CellStairsDown:
		mslog("Going down");
		return GoingDown;
	case CellPit:
		mslog("You falling down!");
		return DropDown;
	}
	mslog(0);
	setcamera(i1);
	hearnoises();
	return PassSegment;
}

void game::action::rotate(direction_s direction) {
	auto i = getcamera();
	auto d = getdirection();
	setcamera(i, rotateto(d, direction));
	hearnoises();
}

static int find_index(int** items, int* itm) {
	for(int i = 0; i < 12; i++)
		if(items[i] == itm)
			return i;
	return -1;
}

int get_potion_duration() {
	return 60 + dice::roll(1, 6) * 10;
}

bool game::action::use(item* pi) {
	unsigned short forward_index = moveto(getcamera(), getdirection());
	auto pc = gethero(pi);
	if(!pc || pc->gethits() <= 0)
		return false;
	auto slot = getitempart(pi);
	if(!pc->isuse(*pi)) {
		pc->say("I don't know what to do with this");
		return false;
	}
	// Weapon is special case
	if((slot == RightHand || slot == LeftHand)) {
		if((!(*pi) || pi->ismelee())) {
			attack(forward_index);
			return true;
		} else if(pi->isranged()) {
			auto index = location.gettarget(getcamera(), getdirection());
			if(index != Blocked)
				attack(index);
			return true;
		}
	}
	spell_s spell_element;
	bool firsttime;
	char name[128]; pi->getname(name, zendof(name));
	bool consume = true;
	auto type = pi->gettype();
	auto po = location.getoverlay(getcamera(), getdirection());
	switch(type) {
	case PotionBlue:
	case PotionGreen:
	case PotionRed:
		if(pi->iscursed()) {
			// RULE: Cursed potion always apply strong poison
			pc->add(StrongPoison, xrand(2, 6) * 4, NoSave);
		} else {
			switch(pi->getenchant()) {
			case OfPoison:
				pc->add(WeakPoison, xrand(2, 8) * 4, NoSave);
				break;
			case OfHealing:
				pc->damage(Magic, -dice::roll(1 + pi->getmagic(), 4) + 2);
				break;
			case OfRegeneration:
				pc->damage(Magic, -dice::roll(1 + pi->getmagic(), 8) + 5);
				break;
			case OfFireResistance:
				pc->set(FireResisted, get_potion_duration());
				break;
			case OfStrenght:
				pc->set(Strenghted, get_potion_duration());
				break;
			case OfInvisibility:
				pc->set(Invisibled, get_potion_duration());
				break;
			case OfNeutralizePoison:
				pc->set(WeakPoison, 0);
				pc->set(Poison, 0);
				pc->set(StrongPoison, 0);
				pc->set(DeadlyPoison, 0);
				break;
			case OfSpeed:
				pc->set(Hasted, get_potion_duration());
				break;
			}
		}
		break;
	case Ration:
	case RationIron:
		if(pi->isbroken()) {
			mslog("%1 is not eadible!", name);
			return false;
		}
		if(!draw::dlgask("Do you want make camp?"))
			return false;
		game::action::camp(*pi);
		break;
	case MagicBook:
	case HolySymbol:
		consume = false;
		spell_element = game::action::choosespell(pc, (type == HolySymbol) ? Cleric : Mage);
		if(!spell_element)
			return false;
		pc->cast(spell_element, (type == HolySymbol) ? Cleric : Mage, 0);
		break;
	case TheifTools:
		consume = false;
		firsttime = false;
		if(location.get(forward_index) == CellPit) {
			if(pc->use(RemoveTraps, forward_index, 15, &firsttime, 100, true)) {
				location.set(forward_index, CellPassable);
				mslog("You remove pit");
			}
		} else if(location.get(forward_index) == CellButton) {
			if(pc->use(RemoveTraps, forward_index, 0, &firsttime, 100, true)) {
				location.set(forward_index, CellPassable);
				mslog("You remove trap");
			}
		} else if(po && po->type == CellTrapLauncher) {
			if(location.isactive(po))
				pc->say("This trap already disabled");
			else if(pc->use(RemoveTraps, forward_index, 0, &firsttime, 100, true)) {
				location.setactive(po, true);
				mslog("You disable trap");
			}
		} else if(po && (po->type == CellKeyHole1 || po->type == CellKeyHole2)) {
			if(location.isactive(po))
				pc->say("This lock already open");
			else if(pc->use(OpenLocks, forward_index, 0, &firsttime, 100, true)) {
				location.setactive(po, true);
				mslog("You pick lock");
			}
		} else {
			pc->say("This usable on pit, lock or trap");
			return false;
		}
		if(d100() < chance_broke_instrument) {
			mslog("You broke %1", name);
			consume = true;
		}
		break;
	case MagicWand:
		consume = false;
		spell_element = pi->getspell();
		if(!spell_element)
			return false;
		if(pi->getcharges()) {
			int magic = pi->getmagic();
			if(magic <= 0)
				magic = 1;
			if(pc->cast(spell_element, Mage, magic)) {
				pi->setidentified(1);
				pi->setcharges(pi->getcharges() - 1);
				if(pi->iscursed()) {
					// RULE: Cursed wands want drawback
					if(d100() < 30)
						pc->damage(Pierce, dice::roll(1, 6));
				}
			} else {
				char name[32];
				mslog("Nothing happened, when %1 try use wand", pc->getname(name, zendof(name)));
			}
		}
		if(!pi->getcharges())
			consume = true;
		break;
	case DungeonMap:
		forward_index = location.getsecret();
		if(forward_index) {
			static const char* speech[] = {
				"Aha! This map show secret door!",
				"I know this! Map show me secret door!",
			};
			pc->say(maprnd(speech));
			draw::animation::appear(location, forward_index, 2);
		}
		break;
	case MageScroll:
	case PriestScroll:
		consume = false;
		if(!pi->isidentified()) {
			pc->say("When camping try to identify this");
			return false;
		}
		spell_element = pi->getspell();
		if(!spell_element)
			return false;
		else {
			int magic = pi->getmagic();
			if(magic <= 0)
				magic = 1;
			auto cls = (type == MageScroll) ? Mage : Cleric;
			if(pc->cast(spell_element, cls, magic))
				consume = true;
			else {
				char name[64];
				mslog("Nothing happened, when %1 try use scroll", pc->getname(name, zendof(name)));
			}
		}
		break;
	case KeyShelf: case KeySilver: case KeyCooper: case KeySkull: case KeySpider:
	case KeyMoon: case KeyDiamond: case KeyGreen:
		if(po && (po->type == CellKeyHole1 || po->type == CellKeyHole2)) {
			if(location.getkeytype(po->type) == type) {
				location.setactive(po, true);
				creature::addexp(100, 0);
				mslog("You open lock");
			} else {
				pc->say("This does not fit");
				return false;
			}
		} else {
			pc->say("This must be used on key hole");
			return false;
		}
		break;
	default:
		return false;
	}
	if(consume)
		pi->clear();
	return true;
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

creature* get_most_damaged() {
	creature* result = 0;
	int difference = 0;
	for(auto e : game::party) {
		if(!e)
			continue;
		int hp = e->gethits();
		int mhp = e->gethitsmaximum();
		if(hp == mhp)
			continue;
		auto n = mhp - hp;
		if(n > difference) {
			result = e;
			difference = n;
		}
	}
	return result;
}

static void try_autocast(creature* pc) {
	spell_s healing_spells[] = {SpellCureLightWounds, LayOnHands};
	for(auto e : healing_spells) {
		if(!pc->get(e))
			continue;
		auto target = get_most_damaged();
		if(!target)
			continue;
		pc->cast(e, Cleric, 0, target);
	}
}

void game::action::camp(item& it) {
	for(auto e : game::party) {
		if(!e)
			continue;
		if(!e->isready())
			continue;
		try_autocast(e);
	}
	passtime(60 * 8);
	auto food = it.gettype();
	auto poisoned = it.iscursed();
	if(poisoned)
		mslog("Food was poisoned!");
	for(auto pc : game::party) {
		if(!pc)
			continue;
		// RULE: Ring of healing get addition healing
		auto healed = pc->getbonus(OfHealing) * 3;
		if(poisoned) {
			// RULE: Cursed food add weak poison
			pc->add(WeakPoison);
		} else {
			switch(food) {
			case Ration:
				healed += xrand(1, 3);
				break;
			case RationIron:
				healed += xrand(1, 6);
				break;
			}
		}
		pc->damage(Magic, -healed);
		pc->preparespells();
		// Recharge some items
		for(auto i = FirstInvertory; i <= LastInvertory; i = (wear_s)(i + 1)) {
			auto pi = pc->getitem(i);
			if(!pi || !(*pi))
				continue;
			auto type = pi->gettype();
			switch(type) {
			case MagicWand:
				if(pi->iscursed())
					break;
				// RULE: Only mages can recharge spells
				if(pc->get(Mage) < 5)
					break;
				if(pi->getcharges() < 50)
					pi->setcharges(pi->getcharges() + 1);
				break;
			case MageScroll:
			case PriestScroll:
				// Autodetect scrolls by itellegence check
				if((type == MageScroll && pc->get(Mage))
					|| (type == PriestScroll && (pc->get(Cleric) || pc->get(Paladin) || pc->get(Ranger)))
					|| pc->get(Theif) >= 3) {
					if(pi->isidentified())
						break;
					if(pc->roll(LearnSpell)) {
						char name[128];
						pi->setidentified(1);
						pc->say("It's %1", pi->getname(name, zendof(name)));
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

int game::getside(int side, direction_s dr) {
	if(dr == Center)
		return side;
	return place_sides[dr - Left][side];
}

static item* find_item_to_get(short unsigned index, int side) {
	item* result[2];
	int count = location.getitems(result, zendof(result), game::getcamera(), side);
	if(!count)
		count = location.getitems(result, zendof(result), game::getcamera());
	if(!count)
		return 0;
	return result[0];
}

static int autodetect_side(item* itm) {
	auto pc = game::gethero(itm);
	if(!pc)
		return 0;
	int n = zfind(game::party, pc);
	return n == -1 ? 0 : (n % 2);
}

void game::action::getitem(item* itm, int side) {
	char temp[260];
	if(!itm || *itm)
		return;
	if(side == -1)
		side = autodetect_side(itm);
	auto gitm = find_item_to_get(getcamera(), getside(side, getdirection()));
	if(!gitm)
		return;
	auto slot = getitempart(itm);
	auto pc = gethero(itm);
	if(!pc->isallow(*gitm, slot))
		return;
	iswap(*itm, *gitm);
	mslog("%1 picked up", itm->getname(temp, zendof(temp)));
}

void game::action::dropitem(item* pi, int side) {
	auto pc = gethero(pi);
	if(!pc)
		return;
	if(!pi || !(*pi))
		return;
	if(side == -1)
		side = autodetect_side(pi);
	auto s1 = getitempart(pi);
	if(!pc->isallowremove(*pi, s1, true))
		return;
	char temp[260]; ;
	mslog("%1 dropped", pi->getname(temp, zendof(temp)));
	location.dropitem(getcamera(), *pi, getside(side, getdirection()));
	pi->clear();
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

void game::action::attack(short unsigned index_of_monsters) {
	creature* parcipants[13];
	auto dr = getdirection();
	location.turnto(index_of_monsters, vectorized(dr, Down));
	draw::animation::update();
	select_parcipants(parcipants, index_of_monsters);
	roll_inititative(parcipants);
	for(int i = 0; parcipants[i]; i++) {
		auto attacker = parcipants[i];
		if(!attacker->isready())
			continue;
		attacker->attack(index_of_monsters, dr, 0);
	}
	// RULE: Hasted units make second move at end of combat round
	for(int i = 0; parcipants[i]; i++) {
		auto attacker = parcipants[i];
		if(!attacker->isready())
			continue;
		if(attacker->is(Hasted)
			|| attacker->getbonus(OfSpeed, Legs)
			|| attacker->getbonus(OfSpeed, Elbow))
			attacker->attack(index_of_monsters, dr, 0);
	}
}

unsigned creature::select(spell_s* result, spell_s* result_maximum, class_s type, int level) {
	auto p = result;
	for(auto rec = NoSpell; rec < FirstSpellAbility; rec = (spell_s)(rec + 1)) {
		if(getlevel(rec, type) != level)
			continue;
		*p++ = rec;
	}
	return p - result;
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
		location.set(moveto(index, dr), CellPassable);
		location.remove(po);
		pc->say("This is secret door");
		break;
	case CellCellar:
		if(*itm) {
			if(game::getsize(itm->gettype()) == Large) {
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
			if(po->active)
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

int game::getpartyskill(int rec, skill_s id) {
	int count = 0;
	int total = 0;
	for(auto pc : party) {
		if(!pc)
			continue;
		total += pc->get(id);
		count++;
	}
	if(!count)
		return 0;
	return total / count;
}

int game::getarmorpenalty(item_s armor, skill_s skill) {
	switch(armor) {
	case ArmorStuddedLeather:
		switch(skill) {
		case OpenLocks: return 10;
		case RemoveTraps: return 15;
		case ClimbWalls: return 30;
		default: return 20;
		}
		break;
	case ArmorChain:
	case ArmorScale:
		return 40;
	case ArmorPlate:
		return 80;
	default:
		return 0;
	}
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

static bool is_valid_move_by_size(creature** s_side, creature** d_side) {
	if(!s_side[0])
		return true;
	if(s_side[0]->getsize() < Large)
		return true;
	return !d_side[0] && !d_side[1] && !d_side[2] && !d_side[3];
}

static void stop_monster(short unsigned index) {
	creature* s_side[4]; location.getmonsters(s_side, index, Center);
	for(auto pc : s_side) {
		if(pc)
			pc->setmoved(true);
	}
}

static void move_monster(dungeon& location, short unsigned index, direction_s dr) {
	auto to = moveto(index, dr);
	if(location.isblocked(to))
		return;
	if(location.get(to) == CellPit)
		return;
	if(to == game::getcamera())
		return;
	creature* s_side[4]; location.getmonsters(s_side, index, Center);
	creature* d_side[4]; location.getmonsters(d_side, to, Center);
	if(index == to) {
		stop_monster(index);
		return;
	}
	// Large monsters move only to free index
	if(!is_valid_move_by_size(s_side, d_side) || !is_valid_move_by_size(d_side, s_side))
		return;
	// Medium or smaller monsters
	// can be mixed on different sides
	for(int i = 0; i < 4; i++) {
		auto pc = s_side[i];
		if(!pc)
			continue;
		pc->setmoved(true);
		auto s = i;
		if(d_side[s]) {
			s = game::getfreeside(d_side);
			if(s == -1)
				continue;
		}
		d_side[s] = pc;
		s_side[i] = 0;
		pc->setside(s);
		pc->setindex(to);
		pc->set(dr);
	}
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
		if(e.is(Scared)) {
			direction_s free_directions[] = {rotateto(party_direct, Up), rotateto(party_direct, Left), rotateto(party_direct, Left), Center};
			auto free_direct = location.getpassable(monster_index, free_directions);
			if(monster_direct != free_direct)
				location.turnto(monster_index, free_direct);
			if(free_direct)
				move_monster(location, monster_index, free_direct);
		} else if(moveto(monster_index, monster_direct) == party_index) {
			mslog("You are under attack!");
			location.turnto(party_index, vectorized(monster_direct, Down));
			game::action::attack(monster_index);
		} else if(d100() < 45) {
			auto next_index = moveto(monster_index, monster_direct);
			if(location.isblocked(next_index) || d100() < 30) {
				short unsigned indicies[5];
				auto n = location.random(location.getnearestfree(indicies, monster_index));
				if(n)
					location.turnto(monster_index, pointto(monster_index, n));
			} else
				move_monster(location, monster_index, monster_direct);
		} else
			stop_monster(monster_index);
	}
	// Обновим монстров
	for(auto& e : location.monsters) {
		if(e)
			e.update(false);
	}
	// Обновим героев
	for(auto pc : game::party) {
		if(!pc)
			continue;
		pc->update(true);
	}
}

bool get_secret(short unsigned index, direction_s sight_dir, direction_s rotate_dir, direction_s& secret_dir) {
	auto po = location.getoverlay(index, rotateto(sight_dir, rotate_dir));
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

void game::hearnoises() {
	direction_s secret_dir = Center;
	auto index = getcamera();
	auto dir = getdirection();
	auto door_index = moveto(index, dir);
	if(!door_index || location.get(door_index) != CellDoor)
		return;
	if(location.is(door_index, CellActive))
		return;
	door_index = moveto(door_index, dir);
	if(!door_index)
		return;
	for(auto pc : game::party) {
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

void game::passtime(int minutes) {
	while(minutes > 0) {
		passround();
		int count = 3;
		if(count > minutes)
			count = minutes;
		minutes -= count;
		game::rounds += count;
	}
}

static bool is_anybody_live() {
	for(auto e : game::party) {
		if(!e)
			continue;
		if(e->isready())
			return true;
	}
	return false;
}

static void falling_damage() {
	for(auto e : game::party) {
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
	auto index = game::getcamera();
	location.getmonsters(monsters, index, game::getdirection());
	for(auto e : monsters) {
		if(!e)
			continue;
		e->clear();
	}
}

command_s game::action::adventure() {
	while(true) {
		if(!is_anybody_live())
			return GameOver;
		auto id = game::action::actions();
		switch(id) {
		case NoCommand:
			break;
		case PassSegment:
			rounds++;
			passround();
			findsecrets();
			break;
		case GoingUp:
			write();
			if(location.level <= 1)
				return LeaveArea;
			enter(location.overland_index, location.level - 1);
			game::setcamera(moveto(location.stat.down.index, location.stat.down.dir), location.stat.down.dir);
			break;
		case GoingDown:
			write();
			enter(location.overland_index, location.level + 1);
			game::setcamera(moveto(location.stat.up.index, location.stat.up.dir), location.stat.up.dir);
			break;
		case DropDown:
			write();
			setcamera(moveto(getcamera(), getdirection()));
			draw::animation::update();
			falling_damage();
			enter(location.overland_index, location.level + 1);
			falling_landing();
			break;
		default:
			return id;
		}
	}
}

void game::enter(unsigned short index, unsigned char level) {
	bool random_ceiling = false;
	overland_index = index;
	location_level = level;
	if(!location.read(overland_index, location_level))
		location.generate(AreaSewers, overland_index, location_level);
	location.link();
	if(location_level > 1)
		location_above.read(overland_index, location_level - 1);
	else
		location_above.clear();
	draw::settiles(location.type);
	if(!camera_index)
		game::setcamera(moveto(location.stat.up.index, location.stat.up.dir), location.stat.up.dir);
}

bool creature::set(skill_s skill, short unsigned index) {
	for(auto& e : events) {
		if(e.pc == this
			&& e.skill == skill
			&& e.index == index)
			return false;
		if(!e.pc) {
			e.pc = this;
			e.skill = skill;
			e.index = index;
			e.rounds = game::rounds;
			return true;
		}
	}
	return false;
}

template<> void archive::set<event_info>(event_info& e) {
	set(e.pc);
	set(e.index);
	set(e.rounds);
	set(e.skill);
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

template<> void archive::set<dungeon>(dungeon& e) {
	set(e.type);
	set(e.overland_index);
	set(e.level);
	set(e.stat);
	set(e.haspits);
	set(e.data);
	set(e.items);
	set(e.overlays);
	set(e.monsters);
	set(events);
}

static bool serialize(dungeon& e, short unsigned overland_index, int level, bool write_mode) {
	char temp[260];
	io::file file(fname(temp, overland_index, level), write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive::dataset pointers[] = {hero_data};
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