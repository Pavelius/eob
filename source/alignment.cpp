#include "main.h"

alignmenti bsdata<alignmenti>::elements[] = {
	{"Lawful Good", Lawful, Good, {Theif}, 4},
	{"Neutral Good", Neutral, Good, {Paladin}, 3},
	{"Chaotic Good", Chaotic, Good, {Paladin}, 1},
	{"Lawful Neutral", Lawful, Neutral, {Theif, Ranger, Paladin}, 1},
	{"Neutral", Neutral, Neutral, {Ranger, Paladin}, 0},
	{"Chaotic Neutral", Chaotic, Neutral, {Ranger, Paladin}, -1},
	{"Lawful Evil", Lawful, Evil, {Theif, Ranger, Paladin}, -2},
	{"Neutral Evil", Neutral, Evil, {Ranger, Paladin}, -3},
	{"Chaotic Evil", Chaotic, Evil, {Ranger, Paladin}, -4},
};
assert_enum(alignmenti, ChaoticEvil)
BSDATAF(alignmenti)

moralei bsdata<moralei>::elements[] = {
	{"Lawful", 1},
	{"Good", 1},
	{"Neutral"},
	{"Chaotic", -1},
	{"Evil", -1},
};
assert_enum(moralei, Evil)
BSDATAF(moralei)