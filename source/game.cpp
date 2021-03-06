#include "archive.h"
#include "main.h"

// Dungeon can hold from 1 to 10 levels.

const int save_subversion = 7;
#ifdef _DEBUG
const bool visialize_map = false;
#else
const bool visialize_map = false;
#endif // _DEBUG


static const char place_sides[4][4] = {{1, 3, 0, 2},
{0, 1, 2, 3},
{2, 0, 3, 1},
{3, 2, 1, 0},
};
static const char* name_direction[] = {"floor",
"left", "forward", "right", "rear"};

gamei						game;
creaturea					party;
dungeoni					location_above;
dungeoni					location;

void gamei::setcamera(indext index, direction_s direction) {
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
	return bsdata<creature>::source.indexof(this) != -1;
}

int	gamei::getindex(const creature* p) const {
	if(!this)
		return -1;
	return party.indexof(const_cast<creature*>(p));
}

static int find_index(int** items, int* itm) {
	for(int i = 0; i < 12; i++)
		if(items[i] == itm)
			return i;
	return -1;
}

bool gamei::question(item* current_item) {
	auto pc = current_item->getowner();
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
	for(auto p : party) {
		if(!p || p->gethits() <= 0)
			continue;
		int side = p->getside();
		if(side > 3)
			continue;
		result[side] = p;
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

static void single_attack(creature* p, creature* d, int bonus, int multiplier) {
	auto wp1 = p->get(RightHand);
	auto wp2 = p->get(LeftHand);
	auto wp3 = p->get(Head);
	if(wp1.istwohanded() || !wp2.ismelee())
		wp2 = NoItem;
	if(!wp3.ismelee())
		wp3 = NoItem;
	if(wp2) {
		p->attack(d, RightHand, bonus + p->gethitpenalty(-4), multiplier);
		p->attack(d, LeftHand, bonus + p->gethitpenalty(-6), multiplier);
	} else
		p->attack(d, RightHand, bonus, multiplier);
	if(wp3)
		p->attack(d, Head, bonus, multiplier);
}

static void attack_object(indext index, int hits, cell_s destroyed) {
	creature object;
	object.clear();
	object.set(StaticObject);
	object.finish();
	object.sethits(hits);
	for(auto p : party) {
		if(!p || !(*p))
			continue;
		if(!object.isready())
			break;
		single_attack(p, &object, 0, 1);
	}
	if(!object.isready())
		location.set(index, destroyed);
}

void gamei::attack(indext index_of_monsters, bool ranged, ambush_s ambush) {
	auto tile = location.get(index_of_monsters);
	auto& et = bsdata<celli>::elements[tile];
	if(et.destroyed)
		attack_object(index_of_monsters, 10, et.destroyed);
	creaturea parcipants;
	auto dr = getdirection();
	bool monster_surprise = false;
	location.turnto(index_of_monsters, to(dr, Down), &monster_surprise);
	location.formation(index_of_monsters, to(dr, Down));
	draw::animation::update();
	if(ambush == NoAmbush && monster_surprise)
		ambush = MonsterAmbush;
	if(ambush != PartyAmbush)
		parcipants.select(game.getcamera());
	if(ambush != MonsterAmbush)
		parcipants.select(index_of_monsters);
	parcipants.rollinitiative();
	// Ambush
	if(ambush) {
		auto surprise_message = 0;
		for(auto attacker : parcipants) {
			if(!attacker->isready())
				continue;
			// RULE: surprise depends on MoveSilently
			if(attacker->roll(MoveSilently) || attacker->is(Invisibility)) {
				if(!surprise_message) {
					if(ambush == PartyAmbush)
						mslog("You are surprised!");
					else
						mslog("You catch them surprised!");
				}
				surprise_message++;
				auto theif = attacker->get(Theif);
				if(theif) // RULE: Backstab ability of theif
					attacker->attack(index_of_monsters, dr, 4, ranged, (theif + 7) / 4);
				else
					attacker->attack(index_of_monsters, dr, 4, ranged, 1);
			}
		}
		attack(index_of_monsters, ranged, NoAmbush);
	} else {
		for(auto attacker : parcipants) {
			if(!attacker->isready())
				continue;
			attacker->attack(index_of_monsters, dr, 0, ranged, 1);
		}
		// RULE: Hasted units make second move at end of combat round
		for(auto attacker : parcipants) {
			if(!attacker->isready())
				continue;
			if(attacker->is(Haste))
				attacker->attack(index_of_monsters, dr, 0, ranged, 1);
		}
	}
}

void read_message(dungeoni* pd, dungeoni::overlayi* po);

bool gamei::manipulate(item* itm, direction_s dr) {
	int index = getcamera();
	auto pc = itm->getowner();
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
		game.addexpc(500, 0);
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
	static const char* forest_speech[] = {
		"I see something on %1 trees",
		"Trees on the %1 side seems to be unstable",
	};
	direction_s secret_dir = Center;
	auto index = game.getcamera();
	auto dir = game.getdirection();
	if(!get_secret(index, dir, Left, secret_dir)
		&& !get_secret(index, dir, Right, secret_dir))
		return;
	for(auto p : party) {
		if(!p || !p->isready())
			continue;
		if(p->roll(DetectSecrets)) {
			p->say(maprnd(speech), name_direction[secret_dir]);
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
	auto pc = itm->getowner();
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
		auto p = party[i];
		if(p && p->iscast(type))
			return p;
		if(++i >= party.getmaximum())
			i = 0;
		if(i == stop)
			return 0;
	}
}

bool gamei::isalive() {
	for(auto p : party) {
		if(p && p->isready())
			return true;
	}
	return false;
}

wear_s gamei::getwear(const item* itm) const {
	auto p = itm->getowner();
	if(p)
		return (wear_s)(FirstInvertory + (itm - p->getitem(FirstInvertory)));
	return Head;
}

void gamei::passround() {
	// ������� �� ��������
	for(auto& e : location.monsters) {
		if(!e || !e.isready() || e.ismoved())
			continue;
		auto party_index = game.getcamera();
		auto party_direct = game.getdirection();
		auto monster_index = e.getindex();
		auto monster_direct = e.getdirection();
		if(monster_index == Blocked)
			continue;
		if(to(monster_index, monster_direct) == party_index)
			game.interract(monster_index);
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
	for(auto p : party) {
		if(p)
			p->update(true);
	}
	// Every round update
	creature::update_boost();
	// Slow update
	while(rounds_turn < rounds) {
		for(auto& e : location.monsters) {
			if(e)
				e.update_turn(false);
		}
		for(auto p : party) {
			if(p)
				p->update_turn(true);
		}
		rounds_turn += 10;
	}
	// Hourly update
	while(rounds_hour < rounds) {
		for(auto& e : location.monsters) {
			if(e)
				e.update_hour(false);
		}
		for(auto p : party) {
			if(p)
				p->update_hour(true);
		}
		rounds_hour += 60;
		location.passhour();
	}
	// Every 4 hours update
	while(rounds_daypart < rounds) {
		rounds_daypart += 4 * 60;
		for(auto& e : bsdata<settlementi>())
			e.update();
	}
	if(location)
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

void gamei::enter(variant index, char level) {
	location_index = index;
	location_level = level;
	location.clear();
	location_above.clear();
	if(location_index.type == Adventure) {
		auto pa = getadventure();
		if(!location.read(location_index.value, location_level)) {
			pa->create(visialize_map);
			if(!location.read(location_index.value, location_level))
				return;
		}
		if(location_level > 1)
			location_above.read(location_index.value, location_level - 1);
		draw::settiles(location.head.type);
		setcamera(to(location.stat.up.index, location.stat.up.dir), location.stat.up.dir);
	} else if(location_index.type == Settlement)
		camera_index = Blocked;
	draw::setnext(play);
}

bool creature::set(ability_s skill, short unsigned index) {
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

static bool addstatical(archive& a) {
	if(!a.signature("STD"))
		return false;
	if(!a.version(0, save_subversion))
		return false;
	a.set(textable::getstrings());
	a.set(bsdata<adventurei>::source);
	a.set(bsdata<creature>::source);
	a.set(bsdata<settlementi>::source);
	a.set(bsdata<eventi>::source);
	return true;
}

static bool serialize(bool writemode) {
	io::file file("maps/gamedata.sav", writemode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, writemode);
	if(!a.signature("SAV"))
		return false;
	if(!a.version(0, save_subversion))
		return false;
	if(writemode)
		game.preserial(true);
	a.set(game);
	if(!writemode)
		game.preserial(false);
	a.set(bsdata<boosti>::source);
	return addstatical(a);
}

static bool serialize(dungeoni& e, short unsigned index, char level, bool write_mode) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("maps/%1.6h%2.2h.aum", index, level);
	io::file file(temp, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write_mode);
	if(!a.signature("DNG"))
		return false;
	if(!a.version(0, save_subversion))
		return false;
	a.set(e);
	return true;
}

static bool serialize(const char* name, companyi& e, bool write_mode) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("modules/%1.gmd", name);
	io::file file(temp, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write_mode);
	if(!a.signature("MOD"))
		return false;
	if(!a.version(0, 2))
		return false;
	a.set(e);
	return addstatical(a);
}

void companyi::write(const char* name) {
	serialize(name, *this, true);
}

bool companyi::read(const char* name) {
	auto result = serialize(name, *this, false);
	return result;
}

void dungeoni::write() {
	serialize(*this, overland_index, level, true);
}

bool dungeoni::read(short unsigned overland, char level) {
	return serialize(*this, overland, level, false);
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

void gamei::equiping() {
	for(auto p : party) {
		if(!p)
			continue;
		p->random_equipment(1);
		p->enchant(Identify, 1);
	}
}

void gamei::returntobase() {
	// Clear all items
	for(auto p : party) {
		if(!p)
			continue;
		p->removeloot();
	}
	// Return to settlement
	auto pa = getadventure();
	if(pa) {
		rideto(variant(Settlement, pa->settlement));
		draw::setnext(play);
	}
	location.clear();
	location_above.clear();
}

void gamei::render_worldmap(void* object) {
	auto pg = (gamei*)object;
	point origin;
	draw::fullimage(pg->location_index.getposition(), &origin);
	point pt = pg->location_index.getposition() - origin;
	draw::redmarker(pt.x - 4, pt.y - 4);
	auto pn = pg->location_index.getname();
	if(pn)
		draw::textbc(pt.x, pt.y + 8, pn);
}

void gamei::worldmap() {
	render_worldmap(this);
	draw::pause();
}

void gamei::rideto(variant v) {
	if(location_index == v)
		return;
	// TODO: calculate ride time
	draw::fullimage(location_index.getposition(), v.getposition(), 0);
	location_index = v;
	draw::appear(render_worldmap, this, 1000);
	auto pa = location_index.getadventure();
	if(pa && pa->message_entering)
		answers::message(pa->message_entering);
	enter(location_index, 1);
}

bool gamei::is(variant id) const {
	for(auto p : party) {
		if(!p)
			continue;
		if(p->ismatch(id))
			return true;
	}
	return false;
}

void gamei::addexp(morale_s id, unsigned value) {
	for(auto p : party) {
		if(!p)
			continue;
		if(p->is(id))
			p->addexp(value);
	}
}

void gamei::addexpc(unsigned value, int killing_hit_dice) {
	unsigned count = 0;
	for(auto p : party) {
		if(p && p->isready())
			count++;
	}
	if(count) {
		int value_per_member = imax((unsigned)1, value / count);
		for(auto p : party) {
			if(p && p->isready()) {
				p->addexp(value_per_member);
				if(killing_hit_dice) {
					if(p->get(Fighter) || p->get(Paladin) || p->get(Ranger))
						p->addexp(10 * killing_hit_dice);
				}
			}
		}
	}
}

int	gamei::getaverage(ability_s id) const {
	auto total = 0, count = 0;
	for(auto p : party) {
		if(!p)
			continue;
		if(!p->isready())
			continue;
		total += p->get(id);
		count++;
	}
	if(!count)
		return 0;
	return total / count;
}

bool gamei::roll(int value) {
	auto dice = xrand(1, 20);
	return value < dice;
}

void gamei::clear() {
	memset(this, 0, sizeof(*this));
	camera_index = Blocked;
}

void gamei::preserial(bool writemode) {
	if(writemode) {
		memset(players, 0, sizeof(players));
		for(auto i = 0; i < party.getcount(); i++)
			players[i] = party[i];
	} else {
		party.clear();
		for(auto i = 0; i < 6; i++) {
			auto p = players[i].getcreature();
			if(!p)
				continue;
			party.add(p);
		}
	}
}

void gamei::addgold(int coins) {
	gold += coins;
	if(gold < 0)
		gold = 0;
}

void gamei::startgame() {
	for(auto i = 0; i < 4; i++)
		party.add(&bsdata<creature>::elements[i]);
}

settlementi* gamei::getsettlement() const {
	return location_index.getsettlement();
}

void gamei::jumpto(variant v) {
	location_index = v;
}

adventurei* gamei::getadventure() {
	return location_index.getadventure();
}

static const char* get_power_name(const void* object, stringbuilder& sb) {
	auto p = ((item*)object)->getenchantment();
	if(!p)
		return "Not magical";
	return p->power.getname();
}

void gamei::scriblescrolls() {
	creaturea source;
	for(auto p : party) {
		if(p && p->iscast(Mage))
			source.add(p);
	}
	if(!source) {
		draw::dlgmsg("You don't have any Mage in party.");
		return;
	}
	auto pc = source[0];
	while(true) {
		itema items;
		items.select();
		items.match({MageScroll}, true);
		items.identified(true);
		items.havespell(pc, false);
		bool cancel_button;
		auto pi = items.choose("Scrolls available:", &cancel_button, pc, &source, &pc, get_power_name);
		if(!pi) {
			if(cancel_button)
				break;
		} else {
			pc->scribe(*pi);
			break;
		}
	}
}

void creature::autocast(creaturea& party) {
	spell_s healing_spells[] = {CureLightWounds, LayOnHands, Goodberry, CureSeriousWounds};
	for(auto e : healing_spells) {
		while(get(e) > 0) {
			auto target = party.getmostdamaged();
			if(!target)
				break;
			cast(e, Cleric, 0, target);
		}
	}
}

void gamei::camp(item& it) {
	for(auto p : party) {
		if(!p->isready())
			continue;
		p->autocast(party);
	}
	game.passtime(60 * 8);
	auto food = it.gettype();
	auto poisoned = it.iscursed();
	if(poisoned)
		mslog("Food was poisoned!");
	for(auto p : party) {
		// RULE: Ring of healing get addition healing
		int healed = 0;
		if(poisoned) {
			// RULE: Cursed food add weak poison
			p->add(Poison, Instant, NoSave);
		} else {
			switch(food) {
			case Ration:
				healed += xrand(1, 3);
				break;
			case RationIron:
				healed += xrand(2, 6);
				break;
			}
		}
		p->resting(healed);
	}
}

void gamei::endround() {
	rounds++;
	passround();
	findsecrets();
}

bool gamei::isnight() const {
	auto h = gethour();
	return h >= 22 || h <= 6;
}

void gamei::play() {
	if(!game.isalive())
		draw::setnext(draw::mainmenu);
	else if(game.location_index.type == Adventure)
		game.getadventure()->play();
	else if(game.location_index.type == Settlement)
		game.getsettlement()->play();
	else // Error
		draw::setnext(draw::mainmenu);
}

int	gamei::get(action_s id) const {
	switch(id) {
	case Gold: return gold;
	case Reputation: return reputation;
	case Prosperty:
		if(getsettlement())
			return getsettlement()->prosperty;
		return 0;
	default: return 0;
	}
}

void gamei::createdecks() {
	events_deck.create(Event);
	for(auto& e : bsdata<eventi>()) {
		if(e.is(eventi::Start))
			events_deck.addbottom(&e);
	}
	events_deck.shuffle();
}