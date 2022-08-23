#include "main.h"

BSDATA(damagei) = {
	{"Bludgeon", ResistBludgeon, {}, ImmuneNormalWeapon},
	{"Slashing", ResistSlashing, {}, ImmuneNormalWeapon},
	{"Pierce", ResistPierce, {}, ImmuneNormalWeapon},
	{"Cold", {}, ResistCold},
	{"Electricity"},
	{"Fire", {}, ResistFire},
	{"Magic", {}, ResistMagic},
	{"Heal"},
	{"Paralize"},
	{"Death"},
	{"Petrification"},
};
assert_enum(damagei, Petrification)
BSDATAF(damagei)