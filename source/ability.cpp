#include "main.h"

static const char* num = "%1i";
static const char* per = "%1i%%";

BSDATA(abilityi) = {{"Str", num},
{"Dex", num},
{"Con", num},
{"Int", num},
{"Wis", num},
{"Cha", num},
{"Save vs Paralize", per},
{"Save vs Poison", per},
{"Save vs Traps", per},
{"Save vs Magic", per, {}},
{"Climb Walls", per, {Theif}},
{"Hear Noise", per, {Theif}},
{"Move Silently", per, {Theif, Ranger}},
{"Open Locks", per, {Theif}},
{"Remove Traps", per, {Theif}},
{"Read Languages", per, {Theif}},
{"Learn Spells", per, {Mage}},
//
{"Resist Charm", per},
{"Resist Cold", per},
{"Resist Fire", per},
{"Resist Magic", per},
//
{"Deflect critical", per},
{"Detect secrets", per},
};
assert_enum(ability, LastSkill)
INSTELEM(abilityi)