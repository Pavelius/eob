#include "main.h"

static char reaction_adjustment[] = {
	-7, -6, -4, -3, -2, -1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 2, 3, 3,
	4, 4, 4, 5, 5
};
static char open_doors[] = {
	18, 20, 22, 26, 28, 30, 32, 34, 36, 38,
	40, 42, 44, 46, 48, 50, 54, 58, 62,
	66, 72, 78, 84, 90,
	92, 94, 95
};
static char bend_bars[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	2, 2, 4, 4, 7, 7, 10, 13, 16,
	20, 25, 30, 35, 40, 45, 50,
};
static char system_shock_survival[] = {
	30, 30, 30, 35, 40, 45, 50, 55, 60, 65,
	70, 75, 80, 85, 88, 91, 95, 97, 99
};
static char default_theive_skills[ReadLanguages - ClimbWalls + 1][18] = {
	{40, 85, 86, 87, 88, 90, 92, 94, 96, 98, 99, 99, 99, 99, 99, 99, 99, 99},
	{0, 10, 10, 15, 15, 20, 20, 25, 25, 30, 30, 35, 35, 40, 40, 50, 50, 55},
	{0, 15, 21, 27, 33, 40, 47, 55, 62, 70, 78, 86, 94, 99, 99, 99, 99, 99},
	{0, 25, 29, 33, 37, 42, 47, 52, 57, 62, 67, 72, 77, 82, 87, 92, 97, 99},
	{0, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 99},
	{0, 0, 0, 0, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 80},
};
static char savevs_data[4][5][22] = {
	// Warriors - 0
	{{16, 14, 14, 13, 13, 11, 11, 10, 10, 8, 8, 7, 7, 5, 5, 4, 4, 3, 3, 2, 2, 2},
	{18, 16, 16, 15, 15, 13, 13, 12, 12, 10, 10, 9, 9, 7, 7, 6, 6, 5, 5, 3, 3, 2},
	{17, 15, 15, 14, 14, 12, 12, 11, 11, 9, 9, 8, 8, 6, 6, 5, 5, 4, 4, 3, 3, 2},
	{20, 17, 17, 16, 16, 13, 13, 12, 12, 9, 9, 8, 8, 5, 5, 4, 4, 3, 3, 3, 3, 3},
	{19, 17, 17, 16, 16, 14, 14, 13, 13, 11, 11, 10, 10, 8, 8, 7, 7, 6, 6, 5, 5, 4}},
	// Priest - 1
	{{10, 10, 10, 10, 9, 9, 9, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 2, 2, 2},
	{14, 14, 14, 14, 13, 13, 13, 11, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 6, 6, 6},
	{13, 13, 13, 13, 12, 12, 12, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 5, 5, 5},
	{16, 16, 16, 16, 15, 15, 15, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 8, 8, 8},
	{15, 15, 15, 15, 14, 14, 14, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9, 7, 7, 7}},
	// Rogues - 2
	{{13, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8},
	{14, 14, 14, 14, 14, 12, 12, 12, 12, 10, 10, 10, 10, 8, 8, 8, 8, 6, 6, 6, 6, 4},
	{12, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7},
	{16, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11},
	{15, 15, 15, 15, 15, 13, 13, 13, 13, 11, 11, 11, 11, 9, 9, 9, 9, 7, 7, 7, 7, 5}},
	// Wizards - 3
	{{14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 8},
	{11, 11, 11, 11, 11, 11, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 3},
	{13, 13, 13, 13, 13, 13, 11, 11, 11, 11, 11, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 5},
	{15, 15, 15, 15, 15, 15, 13, 13, 13, 13, 13, 11, 11, 11, 11, 11, 9, 9, 9, 9, 9, 7},
	{12, 12, 12, 12, 12, 12, 10, 10, 10, 10, 10, 8, 8, 8, 8, 8, 6, 6, 6, 6, 6, 4}},
};
static char save_index[] = {
	0, 0,
	1,
	3,
};
static_assert(sizeof(save_index) / sizeof(save_index[0]) == (SaveVsMagic - FirstSave) + 1, "Invalid count of save index elements");

void statable::apply(const item& it, bool use_spell) {
	auto pe = it.getenchantment();
	if(pe && pe->power)
		apply(pe->power, it.iscursed() ? -pe->power.bonus : pe->power.bonus, use_spell);
}

void statable::apply(variant v, int m, bool use_spells) {
	switch(v.type) {
	case Ability:
		if(v.value == Hits)
			hits_rolled += m;
		else if(v.value <= Charisma) {
			if(m > 0) {
				auto a = 16 + m;
				if(ability[v.value] < a)
					ability[v.value] = a;
			} else {
				auto a = 7 + m;
				if(ability[v.value] > a)
					ability[v.value] = a;
			}
		} else {
			m *= bsdata<abilityi>::get(v.value).getmultiplier();
			ability[v.value] += m;
		}
		break;
	case Spell:
		if(use_spells) {
			if(bsdata<spelli>::get(v.value).effect.duration != Instant)
				active_spells.set(v.value);
		}
		break;
	case Feat:
		feats.add((feat_s)v.value);
		break;
	}
}

int statable::getthac0(class_s type, int level) {
	static char thac0_monster[] = {
		0, 1, 1, 3, 3, 5, 5, 7, 7, 9,
		9, 11, 11, 13, 13, 15, 15, 17, 17
	};
	static char thac0_warrior[] = {
		0, 0, 1, 2, 3, 4, 5, 6, 7, 8,
		9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
	};
	static char thac0_priest[] = {
		0, 0, 0, 0, 2, 2, 2, 4, 4, 4,
		6, 6, 6, 8, 8, 8, 10, 10, 10, 12, 12
	};
	static char thac0_rogue[] = {
		0, 0, 0, 1, 1, 2, 2, 3, 3, 4,
		4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9
	};
	static char thac0_wizard[] = {
		0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
		3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6
	};
	switch(type) {
	case NoClass: return maptbl(thac0_monster, level);
	case Fighter: case Paladin: case Ranger: return maptbl(thac0_warrior, level);
	case Theif: return maptbl(thac0_rogue, level);
	case Cleric: return maptbl(thac0_priest, level);
	default: return maptbl(thac0_wizard, level);
	}
}

int statable::getstrex() const {
	auto result = ability[Strenght];
	auto str_exeptional = ability[ExeptionalStrenght];
	if(result > 18)
		result += 6;
	else if(result == 18 && str_exeptional > 0) {
		if(str_exeptional <= 50)
			result += 1;
		else if(str_exeptional <= 75)
			result += 2;
		else if(str_exeptional <= 90)
			result += 3;
		else if(str_exeptional <= 99)
			result += 4;
		else
			result += 5;
	}
	return result;
}

void statable::update_attacks(class_s type, int level) {
	ability[AttackAll] += getthac0(type, level);
}

void statable::update_stats() {
	static char hit_probability[] = {
		-5, -5, -3, -3, -2, -2, -1, -1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 1,
		1, 2, 2, 2, 3,
		3, 4, 4, 5, 6, 7
	};
	static char damage_adjustment[] = {
		-5, -5, -3, -3, -2, -2, -1, -1, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 1, 2,
		3, 3, 4, 5, 6,
		7, 8, 9, 10, 11, 12, 14
	};
	static char reaction_adjustment[] = {
		-7, -6, -4, -3, -2, -1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 2, 2, 3, 3,
		4, 4, 4, 5, 5
	};
	static char defence_adjustment[] = {
		-5, -5, -4, -3, -2, -1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 2, 3, 4, 4,
		4, 5, 5, 5, 6, 6
	};
	static char wisdow_spell_bonus[] = {
		-4, -4, -4, -3, -2, -1, -1, -1, 0, 0,
		0, 0, 0, 0, 0, 1, 2, 3, 4, 5
	};
	static char dwarven_bonus[] = {
		0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
		2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5,
		6, 6, 6, 6, 7
	};
	static char charisma_reaction_bonus[] = {
		-8, -7, -6, -5, -4, -3, -2, -1, 0, 0,
		0, 0, 0, 1, 2, 3, 5, 6, 7, 8,
		9, 10, 11, 12, 13, 14
	};
	static char theive_skills_by_dex[ReadLanguages - ClimbWalls + 1][20] = {
		{-60, -55, -50, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 0, 0, 0, 0, 5, 10, 15},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{-20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -15, -10, -5, 0, 0, 0, 0, 5, 10, 15},
		{-10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -5, 0, 0, 0, 0, 0, 0, 5, 10, 15},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	};
	if(is(NoExeptionalStrenght))
		ability[ExeptionalStrenght] = 0;
	// Ability adjustments
	auto k = getstrex();
	ability[AttackMelee] += maptbl(hit_probability, k);
	ability[AttackRange] += maptbl(reaction_adjustment, ability[Dexterity]);
	ability[DamageMelee] += maptbl(damage_adjustment, k);
	ability[AC] += maptbl(defence_adjustment, ability[Dexterity]);
	ability[ReactionBonus] += maptbl(charisma_reaction_bonus, ability[Charisma]);
	ability[SaveVsParalization] += maptbl(reaction_adjustment, ability[Dexterity]) * 5;
	ability[SaveVsMagic] += maptbl(wisdow_spell_bonus, ability[Wisdow]) * 5;
	// Theive skills
	for(auto i = ClimbWalls; i <= ReadLanguages; i = (ability_s)(i + 1))
		ability[i] += maptbl(theive_skills_by_dex[i - ClimbWalls], ability[Dexterity]);
	// Spells
	if(is(ProtectionFromEvil))
		ability[BonusSave] += 1;
	if(is(Blindness)) {
		ability[AttackAll] -= 4;
		ability[Speed] -= 2;
		ability[AC] -= 4;
	}
	if(is(Bless))
		ability[AttackAll] += 1;
	if(is(Fear))
		ability[AttackAll] -= 4;
	if(is(Haste)) {
		ability[AttackAll] += 2;
		ability[Speed] += 2;
		ability[AC] += 2;
	}
	if(is(Deafness))
		ability[Speed] -= 1;
	if(is(MageArmor))
		ability[AC] += 4;
	if(is(ShieldSpell))
		ability[AC] += 7;
	if(is(Invisibility))
		ability[AttackAll] += 2;
	// One of this
	if(is(Invisibility))
		ability[AC] += 4;
	else if(is(Blur))
		ability[AC] += 3;
	// Feats
	if(is(HolyGrace))
		ability[BonusSave] += 10;
	if(is(BonusSaveVsPoison))
		ability[SaveVsPoison] += maptbl(dwarven_bonus, ability[Constitution]) * 5;
	if(is(BonusSaveVsSpells))
		ability[SaveVsMagic] += maptbl(dwarven_bonus, ability[Constitution]) * 5;
	// Common abilitites
	ability[SaveVsParalization] += ability[BonusSave];
	ability[SaveVsPoison] += ability[BonusSave];
	ability[SaveVsTraps] += ability[BonusSave];
	ability[SaveVsMagic] += ability[BonusSave];
	ability[AttackMelee] += ability[AttackAll];
	ability[AttackRange] += ability[AttackAll];
	ability[DamageMelee] += ability[DamageAll];
	ability[DamageRange] += ability[DamageAll];
	// Total and partial immunitites
	if(is(Undead)) {
		ability[ResistCharm] = 100;
		ability[SaveVsPoison] = 100;
	}
	if(is(FeatherFall))
		ability[ClimbWalls] = 100;
	if(is(ResistFireSpell)) {
		if(ability[ResistFire] < 50)
			ability[ResistFire] = 50;
	}
	if(is(ResistColdSpell)) {
		if(ability[ResistCold] < 50)
			ability[ResistCold] = 50;
	}
}

static int get_save_group(class_s value) {
	switch(value) {
	case Theif: return 2;
	case Mage: return 3;
	case Cleric: return 1;
	default: return 0;
	}
}

static int get_save_thrown(ability_s id, class_s type, const char* levels) {
	auto index = save_index[id - FirstSave];
	auto result = 20;
	for(unsigned i = 0; i < bsdata<classi>::elements[type].classes.count; i++) {
		auto n = levels[i];
		if(n < 1)
			continue;
		auto e = bsdata<classi>::elements[type].classes.data[i];
		auto g = get_save_group(e);
		if(n >= sizeof(savevs_data[0][0]) / sizeof(savevs_data[0][0][0]))
			n = sizeof(savevs_data[0][0]) / sizeof(savevs_data[0][0][0]) - 1;
		auto v = savevs_data[g][index][n];
		if(v && v < result)
			result = v;
	}
	return (21 - result) * 5;
}

static bool allow_skill(ability_s id, class_s type) {
	auto& si = bsdata<abilityi>::elements[id];
	if(!si.match)
		return true;
	for(auto e : bsdata<classi>::elements[type].classes) {
		if(si.match.is(e))
			return true;
	}
	return false;
}

void statable::add(ability_s id, class_s type, const char* levels) {
	if(id>=SaveVsParalization && id<=SaveVsMagic)
		ability[id] += get_save_thrown(id, type, levels);
}

void statable::add(ability_s id, class_s type, int level) {
	if(id >= ClimbWalls && id <= ReadLanguages) {
		if(level > 17)
			level = 17;
		ability[id] += maptbl(default_theive_skills[id - ClimbWalls], level);
	}
}

void statable::add(ability_s id, class_s type) {
	static char chance_learn_spells[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 35,
		40, 45, 50, 55, 60, 65, 70, 75, 85, 95,
		96, 97, 98, 99, 100
	};
	if(!allow_skill(id, type))
		return;
	switch(id) {
	case LearnSpell:
		ability[id] += maptbl(chance_learn_spells, ability[Intellegence]);
		break;
	}
}

static int compare_char(const void* p1, const void* p2) {
	return *((char*)p2) - *((char*)p1);
}

void statable::random_ability(race_s race, gender_s gender, class_s type) {
	char result[8];
	for(auto& e : result)
		e = xrand(1, 6) + xrand(1, 6) + xrand(1, 6);
	qsort(result, sizeof(result) / sizeof(result[0]), sizeof(result[0]), compare_char);
	zshuffle(result, 6);
	int max_position = -1;
	int max_value = 0;
	for(int i = 0; i < 6; i++) {
		if(result[i] > max_value) {
			max_position = i;
			max_value = result[i];
		}
	}
	auto bst_position = bsdata<classi>::elements[type].ability;
	if(max_position != -1)
		iswap(result[max_position], result[bst_position]);
	// Womans more charismatic, Mans more intellegent
	if(bst_position != Intellegence && bst_position != Charisma) {
		if((gender == Male && result[Intellegence] < result[Charisma])
			|| (gender == Female && result[Charisma] < result[Intellegence]))
			iswap(result[Intellegence], result[Charisma]);
	}
	// Check maximum by class
	for(int j = 0; j < 6; j++) {
		int m = bsdata<classi>::elements[type].minimum.data[j];
		if(result[j] < m)
			result[j] = m;
	}
	// Check minimum by race
	for(auto j = 0; j < 6; j++) {
		auto m = bsdata<racei>::elements[race].minimum.data[j];
		if(result[j] < m)
			result[j] = m;
	}
	// Check maximum by race
	for(auto j = 0; j < 6; j++) {
		auto m = bsdata<racei>::elements[race].maximum.data[j];
		if(result[j] > m)
			result[j] = m;
	}
	// Adjust ability
	for(auto j = 0; j < 6; j++)
		result[j] += bsdata<racei>::elements[race].adjustment.data[j];
	// Расставим атрибуты по местам
	for(auto j = 0; j < 6; j++)
		ability[j] = result[j];
	ability[ExeptionalStrenght] = xrand(1, 100);
}

int statable::gethitpenalty(int bonus) const {
	if(is(Ambidextrity))
		return 0;
	auto dex = ability[Dexterity];
	auto bon = maptbl(reaction_adjustment, dex);
	bonus += bon;
	if(bonus > 0)
		bonus = 0;
	return bonus;
}