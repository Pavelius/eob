#include "main.h"

enum {
	PercentMult, WeaponAbility
};

BSDATA(abilityi) = {
	{"Str"},
	{"Dex"},
	{"Con"},
	{"Int"},
	{"Wis"},
	{"Cha"},
	{"Save vs Paralize", {PercentMult}},
	{"Save vs Poison", {PercentMult}},
	{"Save vs Traps", {PercentMult}},
	{"Save vs Magic", {PercentMult}},
	{"Climb Walls", {PercentMult}, {Theif}},
	{"Hear Noise", {PercentMult}, {Theif}},
	{"Move Silently", {PercentMult}, {Theif, Ranger}},
	{"Open Locks", {PercentMult}, {Theif}},
	{"Remove Traps", {PercentMult}, {Theif}},
	{"Read Languages", {PercentMult}, {Theif}},
	{"Learn Spells", {PercentMult}, {Mage}},
	{"Resist Charm", {PercentMult}},
	{"Resist Cold", {PercentMult}},
	{"Resist Fire", {PercentMult}},
	{"Resist Magic", {PercentMult}},
	{"Deflect critical", {PercentMult}},
	{"Detect secrets", {PercentMult}},
	{"AC"},
	{"To hit melee", {WeaponAbility}},
	{"To hit range", {WeaponAbility}},
	{"To hit", {WeaponAbility}},
	{"Damage melee", {WeaponAbility}},
	{"Damage range", {WeaponAbility}},
	{"Damage", {WeaponAbility}},
	{"Speed", {WeaponAbility}},
	{"Bonus Experience", {PercentMult}},
	{"Bonus Saves", {PercentMult}},
	{"Reaction bonus"},
	{"Exeptional Strenght", {PercentMult}},
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

bool abilityi::getmultiplier() const {
	return flags.is(PercentMult) ? 5 : 1;
}