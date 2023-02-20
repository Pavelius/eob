#include "crt.h"
#include "feat.h"

BSDATA(feati) = {
	{"Bonus save vs Poison"},
	{"Bonus save vs Spells"},
	{"Holy Grace"},
	{"Ambidextrity"},
	{"No exeptional strenght"},
	{"Undead"},
	{"Slowest"},
	{"Resist bludgeon"},
	{"Resist slashing"},
	{"Resist pierce"},
	{"Immune normal weapon"},
	{"Immune disease"},
	{"Bonus vs elf weapon"},
	{"Bonus to hit goblinoid"},
	{"Bonus damage with level"},
	{"Bonus AC vs Large"},
	{"Bonus hit points"},
};
assert_enum(feati, BonusHP)
BSDATAF(feati)