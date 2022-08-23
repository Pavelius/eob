#include "main.h"

alignmenti bsdata<alignmenti>::elements[] = {
	{"Lawful Good", Lawful, Good, {Theif}},
	{"Neutral Good", Neutral, Good, {Paladin}},
	{"Chaotic Good", Chaotic, Good, {Paladin}},
	{"Lawful Neutral", Lawful, Neutral, {Theif, Ranger, Paladin}},
	{"Neutral", Neutral, Neutral, {Ranger, Paladin}},
	{"Chaotic Neutral", Chaotic, Neutral, {Ranger, Paladin}},
	{"Lawful Evil", Lawful, Evil, {Theif, Ranger, Paladin}},
	{"Neutral Evil", Neutral, Evil, {Ranger, Paladin}},
	{"Chaotic Evil", Chaotic, Evil, {Ranger, Paladin}},
};
assert_enum(alignmenti, ChaoticEvil)
BSDATAF(alignmenti)

moralei bsdata<moralei>::elements[] = {{"Lawful"},
	{"Good"},
	{"Neutral"},
	{"Chaotic"},
	{"Evil"},
};
assert_enum(moralei, Evil)
BSDATAF(moralei)