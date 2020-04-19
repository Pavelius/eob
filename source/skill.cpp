#include "main.h"

skilli bsdata<skilli>::elements[] = {{"Save vs Paralize"},
{"Save vs Poison", {}, OfPoisonResistance, 10},
{"Save vs Traps"},
{"Save vs Magic"},
//
{"Climb Walls", {Theif}},
{"Hear Noise", {Theif}},
{"Move Silently", {Theif, Ranger}},
{"Open Locks", {Theif}},
{"Remove Traps", {Theif}},
//
{"Read Languages", {Theif}},
{"Learn Spells", {Mage}},
//
{"Resist Charm"},
{"Resist Cold"},
{"Resist Fire", {}, OfFireResistance, 20},
{"Resist Magic", {}, OfMagicResistance, 10},
//
{"Deflect critical"},
{"Detect secrets"},
};
assert_enum(skill, LastSkill);

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
static char theive_skills_by_dex[ReadLanguages - ClimbWalls + 1][20] = {
	{-60, -55, -50, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 0, 0, 0, 0, 5, 10, 15},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-20, -20, -20, -20, -20, -20, -20, -20, -20, -20, -15, -10, -5, 0, 0, 0, 0, 5, 10, 15},
{-10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -5, 0, 0, 0, 0, 0, 0, 5, 10, 15},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};
static char default_theive_skills[ReadLanguages - ClimbWalls + 1][18] = {
	{40, 85, 86, 87, 88, 90, 92, 94, 96, 98, 99, 99, 99, 99, 99, 99, 99, 99},
{0, 10, 10, 15, 15, 20, 20, 25, 25, 30, 30, 35, 35, 40, 40, 50, 50, 55},
{0, 15, 21, 27, 33, 40, 47, 55, 62, 70, 78, 86, 94, 99, 99, 99, 99, 99},
{0, 25, 29, 33, 37, 42, 47, 52, 57, 62, 67, 72, 77, 82, 87, 92, 97, 99},
{0, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 99},
{0, 0, 0, 0, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 80},
};
extern char reaction_adjustment[26];
static char darwen_bonus[] = {
	0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
	2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5,
	6, 6, 6, 6, 7
};
static char widow_spell_bonus[] = {
	-4, -4, -4, -3, -2, -1, -1, -1, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 3, 4, 5
};
static char save_index[] = {
	0, 0,
	1,
	3,
};
static_assert(sizeof(save_index) / sizeof(save_index[0]) == SaveVsMagic + 1, "Invalid count of save index elements");
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
static char chance_learn_spells[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 35,
	40, 45, 50, 55, 60, 65, 70, 75, 85, 95,
	96, 97, 98, 99, 100
};
static char system_shock_survival[] = {
	30, 30, 30, 35, 40, 45, 50, 55, 60, 65,
	70, 75, 80, 85, 88, 91, 95, 97, 99
};

static int get_save_group(class_s value) {
	switch(value) {
	case Theif: return 2;
	case Mage: return 3;
	case Cleric: return 1;
	default: return 0;
	}
}

static int get_save_thrown(skill_s id, class_s type, const char* levels) {
	auto index = save_index[id];
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

static int get_theiv_skill(skill_s id, class_s type, const char* levels) {
	auto result = 0;
	for(unsigned i = 0; i < bsdata<classi>::elements[type].classes.count; i++) {
		auto n = levels[i];
		if(n < 1)
			continue;
		auto e = bsdata<classi>::elements[type].classes.data[i];
		auto m = 0;
		if(bsdata<skilli>::elements[id].allow && !bsdata<skilli>::elements[id].allow.is(e))
			m = default_theive_skills[id - ClimbWalls][0];
		else {
			if(n > 17)
				n = 17;
			m = default_theive_skills[id - ClimbWalls][n];
		}
		if(m > result)
			result = m;
	}
	return result;
}

static bool allow_skill(skill_s id, class_s type) {
	if(!bsdata<skilli>::elements[id].allow)
		return true;
	for(auto e : bsdata<classi>::elements[type].classes) {
		if(bsdata<skilli>::elements[id].allow.is(e))
			return true;
	}
	return false;
}

int	creature::get(skill_s id) const {
	int result = bsdata<racei>::elements[race].skills[id];
	if(bsdata<skilli>::elements[id].multiplier)
		result += getbonus(bsdata<skilli>::elements[id].enchant) * bsdata<skilli>::elements[id].multiplier;
	if(id >= FirstSave && id <= LastSave) {
		auto index = save_index[id];
		auto con = get(Constitution);
		auto dex = get(Dexterity);
		result += get_save_thrown(id, type, levels);
		if(is(HolyGrace))
			result += 10;
		switch(id) {
		case SaveVsPoison:
			if(is(BonusSaveVsPoison))
				result += maptbl(darwen_bonus, con) * 5;
			break;
		case SaveVsTraps:
			result += maptbl(reaction_adjustment, dex) * 5;
			break;
		case SaveVsMagic:
			if(is(BonusSaveVsSpells))
				result -= maptbl(darwen_bonus, con);
			result += maptbl(widow_spell_bonus, get(Wisdow)) * 5;
			break;
		}
	} else if(id >= ClimbWalls && id <= ReadLanguages) {
		auto value = get_theiv_skill(id, type, levels);
		switch(id) {
		case ClimbWalls:
			if(is(FeatherFall))
				result += 100;
			result += getbonus(OfClimbing)*10;
			break;
		}
		if(value <= 0)
			return 0;
		result += value;
		auto dex = get(Dexterity);
		result += maptbl(theive_skills_by_dex[id - ClimbWalls], dex);
		result -= wears[Body].getarmorpenalty(id);
	} else {
		if(!allow_skill(id, type))
			return 0;
		auto str = get(Strenght);
		auto ins = get(Intellegence);
		auto con = get(Constitution);
		switch(id) {
		case LearnSpell:
			result += maptbl(chance_learn_spells, ins);
			break;
		case CriticalDeflect:
			result += wears[Body].getdeflect() * 5;
			result += wears[Head].getdeflect() * 5;
			result += wears[Neck].getdeflect() * 5;
			result += wears[Legs].getdeflect() * 5;
			result += wears[RightHand].getdeflect() * 5;
			result += wears[LeftHand].getdeflect() * 5;
			break;
		case ResistCharm:
			if(is(Undead))
				result += 100;
			break;
		case ResistCold:
			if(is(ResistColdSpell))
				result += 50;
			break;
		}
	}
	if(result > 100)
		result = 100;
	return result;
}