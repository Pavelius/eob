#include "main.h"

BSDATA(abilityi) = {
	{"Str", 1},
	{"Dex", 1},
	{"Con", 1},
	{"Int", 1},
	{"Wis", 1},
	{"Cha", 1},
	{"Save vs Paralize", 5},
	{"Save vs Poison", 5},
	{"Save vs Traps", 5},
	{"Save vs Magic", 5, {}},
	{"Climb Walls", 5, {Theif}},
	{"Hear Noise", 5, {Theif}},
	{"Move Silently", 5, {Theif, Ranger}},
	{"Open Locks", 5, {Theif}},
	{"Remove Traps", 5, {Theif}},
	{"Read Languages", 5, {Theif}},
	{"Learn Spells", 5, {Mage}},
	{"Resist Charm", 10},
	{"Resist Cold", 10},
	{"Resist Fire", 10},
	{"Resist Magic", 10},
	{"Deflect critical", 5},
	{"Detect secrets", 5},
	{"AC", 1},
	{"To hit melee", 1},
	{"To hit range", 1},
	{"To hit", 1},
	{"Damage melee", 1},
	{"Damage range", 1},
	{"Damage", 1},
	{"Speed", 1},
	{"Bonus Experience", 5},
	{"Bonus Saves", 5},
	{"Reaction bonus", 1},
	{"Exeptional Strenght", 20},
};
assert_enum(ability, ExeptionalStrenght)
INSTELEM(abilityi)

bool abilityi::allow(class_s v) const {
	for(auto i = 0; i < 3; i++) {
		auto v1 = creature::getclass(v, i);
		if(v1 == NoClass)
			break;
		if(match.is(v1))
			return true;
	}
	return false;
}