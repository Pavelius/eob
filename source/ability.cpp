#include "main.h"

BSDATA(abilityi) = {
	{"Str"},
	{"Dex"},
	{"Con"},
	{"Int"},
	{"Wis"},
	{"Cha"},
	{"Save vs Paralize"},
	{"Save vs Poison"},
	{"Save vs Traps"},
	{"Save vs Magic"},
	{"Climb Walls", {Theif}},
	{"Hear Noise", {Theif}},
	{"Move Silently", {Theif, Ranger}},
	{"Open Locks", {Theif}},
	{"Remove Traps", {Theif}},
	{"Read Languages", {Theif}},
	{"Learn Spells", {Mage}},
	{"Resist Charm"},
	{"Resist Cold"},
	{"Resist Fire"},
	{"Resist Magic"},
	{"Deflect critical"},
	{"Detect secrets"},
	{"AC"},
	{"To hit melee"},
	{"To hit range"},
	{"To hit"},
	{"Damage melee"},
	{"Damage range"},
	{"Damage"},
	{"Speed"},
	{"Bonus Experience"},
	{"Bonus Saves"},
	{"Reaction bonus"},
	{"Exeptional Strenght"},
};
assert_enum(ability, ExeptionalStrenght)
BSDATAF(abilityi)

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

int abilityi::getmultiplier() const {
	switch(this - bsdata<abilityi>::elements) {
	case SaveVsParalization:
	case SaveVsPoison:
	case SaveVsTraps:
	case SaveVsMagic:
		return 5;
	case ClimbWalls:
	case HearNoise:
	case MoveSilently:
	case OpenLocks:
	case RemoveTraps:
	case ReadLanguages:
		return 5;
	case ResistCharm:
	case ResistCold:
	case ResistFire:
		return 10;
	default:
		return 1;
	}
}