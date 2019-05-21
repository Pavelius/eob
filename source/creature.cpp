#include "archive.h"
#include "view.h"

const char* get_name_part(short rec);

static const int monsters_thac0[] = {
	0, 1, 1, 3, 3, 5, 5, 7, 7, 9,
	9, 11, 11, 13, 13, 15, 15, 17, 17, 19
};
static char hit_probability[] = {
	-6, -5, -3, -3, -2, -2, -1, -1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3,
	4, 4, 5, 6, 7
};
static char	damage_adjustment[] = {
	-5, -4, -2, -1, -1, -1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 3, 4, 6,
	8, 9, 10, 11, 12, 14
};
char reaction_adjustment[] = {
	-7, -6, -4, -3, -2, -1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 2, 3, 3,
	4, 4, 4, 5, 5
};
static char defence_adjustment[] = {
	-5, -5, -4, -3, -2, -1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 3, 4, 4,
	4, 5, 5, 5, 6, 6
};
static char hit_points_adjustment[] = {-4,
-3, -2, -2, -1, -1, -1, 0, 0, 0,
0, 0, 0, 0, 0, 1, 2, 3, 4, 5,
5, 6, 6, 6, 7, 7
};
static char wisdow_bonus_spells[][7] = {{1, 0, 0, 0, 0, 0, 0}, // Wisdow 13
{2, 0, 0, 0, 0, 0, 0}, // Wisdow 14
{2, 1, 0, 0, 0, 0, 0}, // Wisdow 15
{2, 2, 0, 0, 0, 0, 0}, // Wisdow 16
{2, 2, 1, 0, 0, 0, 0}, // Wisdow 17
{2, 2, 1, 1, 0, 0, 0}, // Wisdow 18
{3, 2, 2, 1, 0, 0, 0}, // Wisdow 19
{3, 3, 2, 2, 0, 0, 0}, // Wisdow 20
{3, 3, 3, 2, 1, 0, 0}, // Wisdow 21
{3, 3, 3, 3, 2, 0, 0}, // Wisdow 22
{4, 3, 3, 3, 2, 1, 0}, // Wisdow 23
{4, 3, 3, 3, 3, 2, 0}, // Wisdow 24
{4, 3, 3, 3, 3, 3, 1}, // Wisdow 25
};
static char mgu_lev1[] = {0,
1, 2, 3, 3, 4, 4, 4, 4, 4, 4,
4, 4, 5, 5, 5, 5, 6, 6, 7, 7,
8,
};
static char mgu_lev2[] = {
	0,
	0, 0, 1, 2, 2, 2, 3, 3, 3, 4,
	4, 4, 5, 5, 5, 5, 6, 6, 7, 7,
	8,
};
static char mgu_lev3[] = {
	0,
	0, 0, 0, 0, 1, 2, 2, 3, 3, 3,
	4, 4, 5, 5, 5, 5, 6, 6, 7, 7,
	8,
};
static char mgu_lev4[] = {
	0,
	0, 0, 0, 0, 0, 0, 1, 2, 2, 2,
	3, 4, 4, 4, 5, 5, 5, 6, 7, 7,
	7,
};
static char clr_lev1[] = {
	0,
	1, 1, 2, 2, 2, 2, 2, 2, 3, 3,
	4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
	9,
};
static char racial_move_silently[] = {0, 0, 15, 5, 10, 0};
static char racial_open_locks[] = {0, 5, 0, 0, 10, 0};
static char racial_remove_traps[] = {0, 10, 0, 0, 5, 0};
static char thac0_warrior[21] = {
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
};
static char thac0_priest[21] = {
	0, 0, 0, 0, 2, 2, 2, 4, 4, 4,
	6, 6, 6, 8, 8, 8, 10, 10, 10, 12, 12
};
static char thac0_rogue[21] = {
	0, 0, 0, 1, 1, 2, 2, 3, 3, 4,
	4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9
};
static char thac0_wizard[21] = {
	0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
	3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6
};
static char dwarven_constitution_bonuses[] = {
	0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
	2, 3, 3, 3, 4, 4, 4, 4, 5
};
static int experience_warrior[21] = {
	0, 0, 2000, 4000, 8000, 16000, 32000, 64000, 125000, 250000,
	500000, 750000, 1000000, 1250000, 1500000, 1750000, 2000000, 2250000, 2500000, 2750000, 3000000
};
static int experience_wizard[21] = {
	0, 0, 2500, 5000, 10000, 20000, 40000, 60000, 90000, 135000,
	250000, 375000, 750000, 1125000, 1500000, 1875000, 2250000, 2625000, 3000000, 3375000, 3750000
};
static int experience_priest[21] = {
	0, 0, 1500, 3000, 6000, 13000, 27500, 55000, 110000, 225000,
	450000, 675000, 900000, 1125000, 1350000, 1575000, 1800000, 2025000, 2250000, 2475000, 2700000
};
static int experience_rogue[21] = {
	0, 0, 1250, 2500, 5000, 10000, 20000, 40000, 70000, 110000,
	160000, 220000, 440000, 660000, 880000, 1100000, 1320000, 1540000, 1760000, 1980000, 2200000
};

static struct poison_effect {
	state_s		state;
	char		save;
	short		duration;
	dice		damage[2];
} poison_effects[DeadlyPoison - WeakPoison + 1] = {{WeakPoison, 2, 20, {{0}, {1, 3}}},
{Poison, 0, 20, {{0}, {1, 6}}},
{StrongPoison, -1, 30, {{1, 3}, {1, 8}}},
{DeadlyPoison, -4, 20, {{1, 3}, {3, 6}}},
};

static const poison_effect* find_poison(state_s id) {
	for(auto& e : poison_effects) {
		if(e.state == id)
			return &e;
	}
	return 0;
}

static int* get_experience_table(class_s cls) {
	switch(cls) {
	case Cleric: return experience_priest;
	case Fighter: case Paladin: case Ranger: return experience_warrior;
	case Mage: return experience_wizard;
	default: return experience_rogue;
	}
}

static void update_poison(creature* pc, bool interactive) {
	auto hits = 0;
	for(auto& pe : poison_effects) {
		if(pc->is(pe.state)) {
			if(pc->roll(SaveVsPoison, pe.save))
				hits += pe.damage[1].roll();
			else if(pe.damage[0].c)
				hits += pe.damage[0].roll();
		}
	}
	if(hits > 0) {
		if(interactive) {
			char temp[260];
			mslog("%1 feel poison", pc->getname(temp, zendof(temp)));
		}
		draw::animation::update();
		pc->damage(hits);
	}
}

bool creature::add(state_s type, unsigned duration, save_s save) {
	const poison_effect* pe;
	auto race = getrace();
	if(!duration)
		duration = xrand(2, 8);
	switch(type) {
	case StateParalized:
	case StateSleeped:
		// Elf has 90% immunity to paralization, sleep and charm
		if(race == Elf) {
			if(d100() < 90)
				return false;
		}
		if(save == SaveNegate && roll(SaveVsParalization))
			return false;
		return set(type, duration);
	default:
		pe = find_poison(type);
		if(pe) {
			if(save == SaveNegate && roll(SaveVsPoison, pe->save))
				return false;
			return set(pe->state, pe->duration);
		}
		break;
	}
	return false;
}

int creature::gethd() const {
	if(kind)
		return bsmeta<monster_info>::elements[kind].hd[0];
	return levels[0];
}

int	creature::getawards() const {
	return bsmeta<monster_info>::elements[kind].getexperience();
}

size_s creature::getsize() const {
	if(kind)
		return bsmeta<monster_info>::elements[kind].size;
	return Medium;
}

resource_s creature::getres() const {
	if(kind)
		return bsmeta<monster_info>::elements[kind].rfile;
	return NONE;
}

static int getweapon(wear_s weapon) {
	switch(weapon) {
	case LeftHand: return 1;
	case Head: return 2;
	default: return 0;
	}
}

void creature::get(weaponi& result, wear_s weapon, creature* enemy) const {
	int r = 0;
	if(is(StateHasted))
		r += 2;
	if(is(StateBlessed))
		r += 1;
	result.thac0 += r;
	result.thac0 += maptbl(monsters_thac0, ((int)bsmeta<monster_info>::elements[type].hd[0]));
	if(wears[weapon]) {
		wears[weapon].get(result, enemy);
		// RULE: holy weapon do more damage to undead
		if(enemy->is(Undead)) {
			auto b = wears[weapon].get(OfHolyness);
			result.thac0 += b;
			result.damage.b += b;
		}
	}
	else
		result.damage = {1, 2};
	auto race = getrace();
	auto t = getbestclass();
	auto k = get(Strenght);
	result.thac0 += getthac0(t, get(t));
	result.thac0 += maptbl(hit_probability, k);
	if(is(BonusVsElfWeapon) && wears[weapon].is(UseTheifWeapon))
		result.thac0++;
	// Weapon secialist get bonus to hit (only to main hand?)
	if(getspecialist(wears[weapon].gettype()))
		result.thac0++;
}

void creature::add(item value) {
	for(auto i = Backpack; i <= LastBackpack; i = (wear_s)(i + 1)) {
		auto pi = getitem(i);
		if(!pi || *pi)
			continue;
		*pi = value;
		break;
	}
}

bool creature::is(state_s id) const {
	return states[id] && states[id] > game::rounds;
}

bool creature::set(state_s id, unsigned rounds) {
	auto current_value = states[id];
	auto newbe_value = game::rounds + rounds;
	if(newbe_value < current_value)
		return false;
	states[id] = newbe_value;
	return true;
}

item creature::get(wear_s id) const {
	auto pi = ((creature*)this)->getitem(id);
	if(!pi)
		return NoItem;
	return *pi;
}

bool creature::roll(skill_s id, int bonus) {
	auto n = get(id) + bonus * 5;
	if(n <= 0)
		return false;
	auto d = d100();
	return d <= n;
}

bool creature::isinvisible() const {
	return is(StateInvisible) || getbonus(OfInvisibility);
}

bool creature::isready() const {
	return gethits() > 0
		&& !is(StateParalized)
		&& !is(StateSleeped);
}

void creature::update(bool interactive) {
	moved = false;
	if((game::rounds % 4) == 0)
		update_poison(this, interactive);
	// Slow ability
	if((game::rounds % 10) == 0) {
		for(auto slot = Head; slot <= Legs; slot = (wear_s)(slot + 1)) {
			auto pi = getitem(slot);
			if(pi && *pi) {
				auto magic = pi->getmagic();
				switch(pi->getenchant()) {
				case OfRegeneration:
					damage(-magic);
					break;
				}
			}
		}
	}
	update_levelup(interactive);
}

bool creature::isenemy(creature* target) const {
	return ishero() != target->ishero();
}

void creature::say(const char* format, ...) {
	return sayv(format, xva_start(format));
}

void creature::sayv(const char* format, const char* vl) {
	char name[64]; getname(name, zendof(name));
	char message[1024]; szprintv(message, zendof(message), format, vl);
	mslog("%1 \"%2\"", name, message);
}

bool creature::canspeak(race_s language) const {
	if(language == Human)
		return true;
	if(is(StateSpeakable))
		return true;
	auto race = getrace();
	if(race == language)
		return true;
	return false;
}

void creature::equip(item it) {
	static const wear_s slots[] = {Head, Neck, Body, RightRing, LeftRing, Elbow, Legs, Quiver, RightHand, LeftHand};
	for(auto slot : slots) {
		auto pi = getitem(slot);
		if(!pi || *pi)
			continue;
		if(isallow(it, slot)) {
			*pi = it;
			return;
		}
	}
	add(it);
}

int	creature::getbonus(enchant_s id, wear_s slot) const {
	if(kind) {
		if(slot == RightHand && bsmeta<monster_info>::elements[type].is(id))
			return 2;
		return 0;
	}
	return wears[slot].get(id);
}

bool creature::is(state_s id, wear_s slot) const {
	return slot == RightHand
		&& bsmeta<monster_info>::elements[type].is(id);
}

void creature::attack(creature* defender, wear_s slot, int bonus) {
	weaponi wi = {0}; get(wi, slot, defender);
	auto ac = defender->getac();
	// RULE: invisible characters hard to hit and more likely to hit
	if(defender->isinvisible())
		ac += 4;
	if(isinvisible())
		wi.thac0 += 4;
	// RULE: Dwarf can hit goblinoid by 5% better that others
	if(is(BonusToHitVsGoblinoid) && defender->race == Goblinoid)
		wi.thac0 += 1;
	// RULE: Ranger add her level to damage humanoid and goblonoids
	if(is(BonusDamageVsEnemy) && (defender->race == Humanoid || defender->race == Goblinoid))
		wi.damage.b += levels[0];
	auto tohit = 20 - (wi.thac0 + bonus) - (10 - ac);
	auto rolls = xrand(1, 20);
	auto hits = -1;
	auto crhit = 20 - wi.critical_range;
	tohit = imax(2, imin(20, tohit));
	if(rolls >= tohit || rolls >= crhit) {
		auto damage = wi.damage;
		hits = damage.roll();
		hits += damaged(defender, slot);
		// RULE: crtitical hit can deflected
		if(rolls >= crhit) {
			// RULE: critical damage depends on weapon and count in dices
			if(!defender->roll(CriticalDeflect)) {
				damage.b = 0; // Only initial dice rolled second time
				damage.c += wi.critical_multiplier;
				hits += damage.roll();
			}
		}
		// RULE: vampiric ability allow user to drain blood and regain own HP
		auto vampirism = getbonus(OfVampirism, slot);
		if(vampirism) {
			auto hits_healed = xrand(1, 4) + vampirism;
			if(hits_healed > hits)
				hits_healed = hits;
			this->damage(-hits_healed);
		}
	}
	// Show result
	draw::animation::attack(this, slot, hits);
	if(hits != -1) {
		defender->set(StateSleeped, 0); // RULE: when attacking sleeping creature she wake up!
		// Poison attack
		for(auto& e : poison_effects) {
			if(is(e.state, slot))
				defender->add(e.state);
		}
		// Paralize attack
		if(is(StateParalized, slot))
			defender->add(StateParalized, xrand(1, 3));
		defender->damage(hits);
	} else
		draw::animation::render();
	draw::animation::update();
	draw::animation::clear();
}

void creature::attack(creature* defender, int bonus) {
	auto wp1 = get(RightHand);
	auto wp2 = get(LeftHand);
	if(wp1.istwohanded() || !wp2.ismelee())
		wp2 = NoItem;
	if(wp1.isranged())
		attack(defender, RightHand, bonus);
	else {
		if(wp2) {
			attack(defender, RightHand, bonus - 4);
			attack(defender, LeftHand, bonus - 6);
		} else
			attack(defender, RightHand, bonus);
		// RULE: weapon specialist make extra attack on same target
		if(((game::rounds % 2) == 0)
			&& getspecialist(wp1.gettype()))
			attack(defender, RightHand, bonus);
	}
}

void creature::addexp(int value) {
	// RULE: ring of Advise
	auto b = getbonus(OfAdvise);
	if(b)
		value += value * b / 20;
	experience += value;
}

int	creature::damaged(const creature* defender, wear_s slot) const {
	auto r = 0, b = 0;
	b = wears[slot].get(OfFire);
	if(b)
		r += xrand(1, 6) + b;
	b = wears[slot].get(OfCold);
	if(b)
		r += xrand(1, 4) + b;
	return r;
}

void creature::clear() {
	memset(states, 0, sizeof(states));
	memset(wears, 0, sizeof(wears));
	memset(prepared, 0, sizeof(prepared));
	memset(known, 0, sizeof(known));
	memset(spells, 0, sizeof(spells));
	moved = false;
	index = 0;
	side = 0;
	direction = Center;
	hits = hits_rolled = initiative = 0;
	kind = NoMonster;
	experience = 0;
}

void creature::finish() {
	feats = bsmeta<race_info>::elements[race].feats;
	//feats.data = bsmeta<class_info>::elements[type].feats;
	usability = bsmeta<race_info>::elements[race].usability;
	usability.data |= bsmeta<class_info>::elements[type].usability.data;
	states[0] = 1;
	for(int i = 0; i < 3; i++) {
		auto c = getclass(getclass(), i);
		if(!c)
			continue;
		raise_level(c);
	}
	sethits(gethitsmaximum());
	if(!kind) {
		setname();
		random_equipmant();
	}
}

void creature::raise_level(class_s type) {
	auto level = get(type);
	if(getclass(this->type, 0) == type)
		levels[0] = level + 1;
	else if(getclass(this->type, 1) == type)
		levels[1] = level + 1;
	else if(getclass(this->type, 2) == type)
		levels[2] = level + 1;
	int hd = bsmeta<class_info>::elements[type].hd;
	if(!hd)
		return;
	auto hp = 1 + (rand() % hd);
	if(level == 0) {
		hp = hd;
		if(type == Mage) {
			setknown(SpellDetectMagic, 1);
			random_spells(type, 1, 3);
			prepare_random_spells(type, 1);
			preparespells();
		} else if(type == Cleric) {
			random_spells(type, 1, 32);
			prepare_random_spells(type, 1);
			preparespells();
		}
	}
	hits_rolled += hp;
}

void creature::random_equipmant() {
	auto cls = getbestclass();
	auto race = getrace();
	switch(cls) {
	case Fighter:
	case Paladin:
		switch(race) {
		case Dwarf:
			wears[RightHand] = item(AxeBattle);
			wears[LeftHand] = item(Shield);
			break;
		case Halfling:
			wears[RightHand] = item(SwordShort);
			break;
		default:
			wears[RightHand] = item(SwordLong);
			break;
		}
		wears[Body] = item(ArmorStuddedLeather);
		break;
	case Ranger:
		wears[RightHand] = item(SwordLong);
		wears[LeftHand] = item(SwordShort);
		wears[Body] = item(ArmorLeather);
		break;
	case Cleric:
		wears[RightHand] = item(Mace);
		wears[Body] = item(ArmorLeather);
		break;
	case Mage:
		wears[RightHand] = item(Dagger);
		wears[Body] = item(Robe);
		break;
	default:
		wears[RightHand] = item(Dagger);
		wears[Body] = item(ArmorLeather);
		break;
	}
	add(RationIron);
	if(get(Mage)) {
		if(!wears[LeftHand])
			wears[LeftHand] = item(MagicBook);
		else
			add(MagicBook);
	}
	if(get(Cleric)) {
		if(!wears[LeftHand])
			wears[LeftHand] = item(HolySymbol);
		else
			add(HolySymbol);
	}
	if(get(Theif))
		add(TheifTools);
	if(get(Fighter))
		add(Ration);
}

void creature::prepare_random_spells(class_s type, int level) {
	adat<spell_s, 64> spells;
	int maximum_spells = getspellsperlevel(type, level);
	int prepared_spells = 0;
	for(auto i = NoSpell; i < FirstSpellAbility; i = (spell_s)(i + 1)) {
		if(!getknown(i))
			continue;
		if(game::getspelllevel(i, type) == level) {
			prepared_spells += get(i);
			spells.add(i);
		}
	}
	zshuffle(spells.data, spells.count);
	if(spells.count > (unsigned)maximum_spells)
		spells.count = maximum_spells;
	for(auto e : spells)
		setprepare(e, 1);
}

void creature::random_spells(class_s type, int level, int count) {
	adat<spell_s, 64> spells;
	spells.count = game::getspells(spells.data, zendof(spells.data), type, level);
	zshuffle(spells.data, spells.count);
	for(auto e : spells) {
		if(count-- <= 0)
			break;
		if(getknown(e))
			continue;
		if(type == Mage && !roll(LearnSpell))
			continue;
		setknown(e, 1);
	}
}

int	creature::get(class_s type) const {
	switch(type) {
	case Cleric:
		switch(this->type) {
		case Cleric:
		case ClericTheif:
			return levels[0];
		case FighterCleric:
			return levels[1];
		default:
			return 0;
		}
	case Fighter:
		switch(this->type) {
		case Fighter:
		case FighterCleric:
		case FighterMage:
		case FighterMageTheif:
		case FighterTheif:
			return levels[0];
		default:
			return 0;
		}
	case Mage:
		switch(this->type) {
		case Mage:
		case MageTheif:
			return levels[0];
		case FighterMage:
		case FighterMageTheif:
			return levels[1];
		default:
			return 0;
		}
	case Theif:
		switch(this->type) {
		case Theif:
			return levels[0];
		case MageTheif:
		case ClericTheif:
		case FighterTheif:
			return levels[1];
		case FighterMageTheif:
			return levels[2];
		default:
			return 0;
		}
	default:
		return (this->type == type) ? levels[0] : 0;
	}
}

int	creature::get(ability_s id) const {
	auto r = 0;
	switch(id) {
	case Strenght:
		r = ability[id];
		if(is(StateStrenghted)) {
			if(r < 18)
				r = 18;
		}
		r += getbonus(OfStrenght);
		return r;
	default:
		return ability[id];
	}
}

int creature::getside() const {
	if(kind)
		return side;
	if(game::party[0] == this)
		return 0;
	else if(game::party[1] == this)
		return 1;
	else if(game::party[2] == this)
		return 2;
	return 3;
}

short creature::gethitsmaximum() const {
	if(kind)
		return hits_rolled + bsmeta<monster_info>::elements[type].hd[1];
	auto hd = gethd();
	auto rl = (int)hits_rolled / getclasscount();
	auto cs = get(Constitution);
	auto md = maptbl(hit_points_adjustment, cs);
	if(md > 2 && !is(BonusHP))
		md = 2;
	rl += md * hd;
	if(rl < hd)
		rl = hd;
	return rl;
}

int creature::getspeed() const {
	auto r = 0;
	auto k = get(Dexterity);
	r += maptbl(reaction_adjustment, k);
	r += wears[RightHand].getspeed();
	r += wears[LeftHand].getspeed();
	r += getbonus(OfSpeed);
	if(is(StateHasted))
		r += 2;
	return r;
}

int creature::getac() const {
	auto r = 0;
	auto k = get(Dexterity);
	r += maptbl(defence_adjustment, k);
	r += wears[Head].getac();
	r += wears[Body].getac();
	r += wears[RightHand].getac();
	r += wears[LeftHand].getac();
	r += getbonus(OfProtection);
	if(kind)
		r += (10-bsmeta<monster_info>::elements[kind].ac);
	if(is(StateHasted))
		r += 2;
	if(is(StateArmored))
		r += 4;
	if(is(StateShielded))
		r += 7;
	return r;
}

int creature::getclasscount() const {
	auto r = bsmeta<class_info>::elements[type].classes.count;
	return r ? r : 1;
}

void creature::update_levelup(bool interactive) {
	auto exp = getexperience() / getclasscount();
	for(auto i = Cleric; i <= Theif; i = (class_s)(i + 1)) {
		auto clv = get(i);
		if(!clv)
			continue;
		auto tbl = get_experience_table(i);
		if(exp >= tbl[clv + 1]) {
			char temp[64];
			raise_level(i);
			mslog("%1 gain level of experience", getname(temp, zendof(temp)));
		}
	}
}

int	creature::getthac0(class_s cls, int level) const {
	switch(cls) {
	case Fighter:
	case Paladin:
	case Ranger:
		return maptbl(thac0_warrior, level);
	case Theif:
		return maptbl(thac0_rogue, level);
	case Cleric:
		return maptbl(thac0_priest, level);
	default:
		return maptbl(thac0_wizard, level);
	}
}

void creature::setname() {
	if(kind)
		return;
	auto race = getrace();
	auto gender = getgender();
	auto need_correct = true;
	if(race == HalfElf) {
		// RULE: Half-elves get names mostly elvish than humans
		if(d100() < 60)
			race = Elf;
		else
			race = Human;
	}
	if(race == Dwarf)
		need_correct = false;
	auto fe = game::getrandom(0, race, gender, -1);
	if(!fe)
		fe = game::getrandom(0, NoRace, gender, -1);
	auto se = game::getrandom(1, race, gender, need_correct ? fe : -1);
	if(!se)
		se = game::getrandom(1, NoRace, gender, need_correct ? fe : -1);
	name[0] = fe;
	name[1] = se;
}

int creature::get_base_save_throw(skill_s st) const {
	auto c = getclass();
	auto rc = getrace();
	auto b1 = getclass(c, 0);
	auto b2 = getclass(c, 1);
	auto b3 = getclass(c, 2);
	auto con = get(Constitution);
	auto n = gethd();
	auto r = 16 - n;
	if(r < 5)
		r = 5;
	if(b1 == Fighter || b2 == Fighter)
		r -= 1;
	if(b1 == Paladin)
		r -= 3;
	if(b1 == Cleric || b2 == Cleric) {
		if(st == SaveVsPoison || st == SaveVsParalization)
			r -= 2;
	}
	if(b1 == Mage || b2 == Mage) {
		if(st == SaveVsMagic)
			r -= 2;
	}
	if(b1 == Theif || b2 == Theif || b3 == Theif) {
		if(st == SaveVsTraps)
			r -= 2;
	}
	if(rc == Dwarf || rc == Halfling) {
		switch(st) {
		case SaveVsPoison:
		case SaveVsMagic:
			r -= maptbl(dwarven_constitution_bonuses, con);
			break;
		}
	}
	if(is(StateProtectedVsEvil))
		r--;
	return (21 - r) * 5;
}

int creature::armor_penalty(skill_s skill) const {
	return game::getarmorpenalty(wears[Body].gettype(), skill);
}

void creature::preparespells() {
	for(auto i = NoSpell; i <= LastSpellAbility; i = (spell_s)(i + 1)) {
		auto level = game::getspelllevel(i, Mage);
		auto count = getprepare(i);
		if((wears[LeftRing].get(OfWizardy) && wears[LeftRing].get(OfWizardy) == level)
			|| (wears[RightRing].get(OfWizardy) && wears[RightRing].get(OfWizardy) == level))
			count *= 2;
		set(i, count);
	}
	if(get(Cleric))
		set(TurnUndead, 2);
	if(get(Paladin))
		set(LayOnHands, 3);
}

int creature::getspellsperlevel(class_s cls, int spell_level) const {
	int m = 0;
	int caster_level = get(cls);
	if(cls == Cleric) {
		int wisdow = get(Wisdow);
		if(wisdow >= 13)
			m = maptbl(wisdow_bonus_spells, wisdow - 13)[spell_level];
	}
	switch(cls) {
	case Mage:
		switch(spell_level) {
		case 1: return maptbl(mgu_lev1, caster_level);
		case 2: return maptbl(mgu_lev2, caster_level);
		case 3: return maptbl(mgu_lev3, caster_level);
		default: return maptbl(mgu_lev4, caster_level);
		}
	case Cleric:
		switch(spell_level) {
		case 1: return maptbl(clr_lev1, caster_level) + m;
		default: return maptbl(mgu_lev4, caster_level) + m;
		}
	}
	return 0;
}

const char* creature::getname(char* result, const char* result_maximum) const {
	if(kind)
		return bsmeta<monster_info>::elements[kind].name;
	szprint(result, result_maximum, "%1%2",
		get_name_part(name[0]), get_name_part(name[1]));
	return result;
}

int creature::getbonus(enchant_s id) const {
	if(bsmeta<monster_info>::elements[type].is(id))
		return 2; // All monsters have enchantment of 2
	auto r = wears[Head].get(id);
	r += wears[Neck].get(id);
	r += wears[Body].get(id);
	r += wears[RightRing].get(id);
	r += wears[LeftRing].get(id);
	r += wears[Elbow].get(id);
	r += wears[Legs].get(id);
	r += wears[LeftHand].get(id);
	r += wears[RightHand].get(id);
	return r;
}

char creature::racial_bonus(char* data) const {
	return data[getrace()];
}

int creature::getspecialist(item_s type) const {
	auto cls = getclass();
	if(cls != Fighter)
		return 0;
	switch(getrace()) {
	case Dwarf: return (type == AxeBattle || type == Mace || type == HammerWar) ? 1 : 0;
	case HalfElf: case Elf: return (type == SwordLong || type == SwordShort || type == Bow) ? 1 : 0;
	case Halfling: return (type == SwordShort) ? 1 : 0;
	default: return (type == SwordLong || type == SwordBastard || type == SwordTwoHanded) ? 1 : 0;
	}
}

void creature::setframe(short* frames, short index) const {
	if(kind) {
		auto po = bsmeta<monster_info>::elements[type].overlays;
		frames[0] = po[0] * 6 + index;
		frames[1] = po[1] ? po[1] * 6 + index : 0;
		frames[2] = po[2] ? po[2] * 6 + index : 0;
		frames[3] = po[3] ? po[3] * 6 + index : 0;
	} else {
		frames[0] = index;
		frames[1] = 0;
	}
}

direction_s creature::getdirection() const {
	if(kind)
		return direction;
	return game::getdirection();
}

short unsigned creature::getindex() const {
	if(kind)
		return index;
	return game::getcamera();
}

bool creature::have_class(aref<class_s> source) const {
	for(auto e : source) {
		if(get(e))
			return true;
	}
	return false;
}

bool creature::isuse(const item v) const {
	auto vu = v.getuse();
	return (vu & usability.data) == vu;
}

void creature::set(direction_s v) {
	if(kind)
		direction = v;
	else
		game::setcamera(getindex(), v);
}

void creature::setside(int value) {
	if(kind)
		side = value;
}

bool creature::isallow(class_s id, race_s r) {
	return bsmeta<class_info>::elements[id].races.is(r);
}

bool creature::isallow(alignment_s id, class_s c) {
	return !bsmeta<alignment_info>::elements[id].restricted.is(c);
}

class_s creature::getclass(class_s id, int pos) {
	auto& e = bsmeta<class_info>::elements[id].classes;
	if(e.count > (unsigned)pos)
		return e.data[pos];
	return NoClass;
}

bool creature::isallow(const item it, wear_s slot) const {
	if(!it)
		return true;
	if(slot >= Backpack && slot <= LastBackpack)
		return true;
	if(slot == LeftHand) {
		// RULE: two-handed items cannot be equiped in left hand
		if(it.istwohanded())
			return false;
		// RULE: no items can be used in left hand when wear two handed item
		auto rh = get(RightHand);
		if(rh.istwohanded())
			return false;
		return isuse(it);
	} else if(slot == RightHand)
		return true;
	if(!isuse(it))
		return false;
	if(slot == LeftRing)
		slot = RightRing;
	return it.getwear() == slot;
}

void creature::damage(int hits) {
	auto c = gethits() - hits;
	if(hits < 0) {
		auto m = gethitsmaximum();
		if(c > m)
			c = m;
		sethits(c);
	} else if(hits == 0) {
		// Nothing to do
	} else {
		sethits(c);
		draw::animation::damage(this, hits);
		if(!ishero()) {
			int hp = gethits();
			// If we kill enemy monster
			if(hp <= 0) {
				// Add experience
				auto hitd = gethd();
				auto value = getawards();
				addexp(value, hitd);
				// Drop items
				auto index = getindex();
				auto side = getside();
				for(auto par = Head; par <= LastBelt; par = (wear_s)(par + 1)) {
					auto it = get(par);
					if(it || !it.getportrait())
						continue;
					if(d100() < 25) {
						// Random magic item
						auto chance_magic = imax(0, imin(65, 15 + hitd * 3));
						location.dropitem(index,
							item(it.gettype(), chance_magic),
							side);
					}
				}
				clear();
			}
		}
	}
}

void creature::addexp(int value, int killing_hit_dice) {
	int count = 0;
	for(auto pc : game::party) {
		if(pc && pc->isready())
			count++;
	}
	if(count) {
		int value_per_member = imax(1, value / count);
		for(auto pc : game::party) {
			if(pc && pc->isready()) {
				pc->addexp(value_per_member);
				if(killing_hit_dice) {
					if(pc->get(Fighter) || pc->get(Paladin) || pc->get(Ranger))
						pc->addexp(10 * killing_hit_dice);
				}
			}
		}
	}
}

bool creature::use(skill_s skill, short unsigned index, int bonus, bool* firsttime, int exp, bool interactive) {
	if(firsttime)
		*firsttime = false;
	if(get(skill) <= 0)
		return false;
	if(skill == HearNoise && !set(skill, index)) {
		if(firsttime)
			*firsttime = true;
		return false;
	}
	if(roll(skill, bonus)) {
		if(exp)
			addexp(exp);
		return true;
	} else {
		if(interactive)
			mslog("You are failed");
	}
	return false;
}

bool creature::swap(item* itm1, item* itm2) {
	static const char* dontwear[2] = {"I don't wear this", "I do not use this"};
	auto p1 = game::gethero(itm1);
	auto s1 = game::getitempart(itm1);
	auto p2 = game::gethero(itm2);
	auto s2 = game::getitempart(itm2);
	auto interactive = p1->ishero() || p2->ishero();
	if(!p1->isallowremove(*itm1, s1, interactive))
		return false;
	if(!p1->isallowremove(*itm2, s2, interactive))
		return false;
	if(!p1->isallow(*itm2, s1)) {
		if(interactive)
			p1->say(dontwear[0]);
		return false;
	}
	if(!p2->isallow(*itm1, s2)) {
		if(interactive)
			p2->say(dontwear[0]);
		return false;
	}
	iswap(*itm1, *itm2);
	return true;
}

bool creature::isallowremove(const item i, wear_s slot, bool interactive) {
	static const char* speech[] = {"It's mine!",
		"Get yours hands off!",
		"I don't leave this",
	};
	if(slot >= Head && slot <= Legs) {
		if(i.iscursed()) {
			if(interactive)
				say(maprnd(speech));
			return false;
		}
	}
	return true;
}