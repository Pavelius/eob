#include "main.h"

static const char* num = "%1i";
static const char* per = "%1i%%";

BSDATA(abilityi) = {{"Str", num, {}, {"strenght", "ogre strenght", "hill giant", "fire giant", "cloud giant"}},
{"Dex", num, {}, {"archery"}},
{"Con", num, {}, {"health"}},
{"Int", num, {}, {"intellegence"}},
{"Wis", num, {}, {"wisdow"}},
{"Cha", num, {}, {"charisma"}},
//
{"Save vs Paralize", per},
{"Save vs Poison", per, {}, {"poison immunity"}, 0, 15},
{"Save vs Traps", per},
{"Save vs Magic", per, {}, {"magic resistance"}},
//
{"Climb Walls", per, {Theif}, {"feather falling"}, 0, 100},
{"Hear Noise", per, {Theif}},
{"Move Silently", per, {Theif, Ranger}, {"elvenkind"}, 0, 90},
{"Open Locks", per, {Theif}, {"bulgary"}, 0, 40},

{"Remove Traps", per, {Theif}},
//
{"Read Languages", per, {Theif}},
{"Learn Spells", per, {Mage}},
//
{"Resist Charm", per, {}, {"paralize immunity"}, 0, 100},
{"Resist Cold", per, {}, {"cold immunity"}, 0, 100},
{"Resist Fire", per, {}, {"fire immunity"}, 0, 100},
{"Resist Magic", per, {}, {"magic resistance"}, 0, 60},
//
{"Deflect critical", per},
{"Detect secrets", per},
};
assert_enum(ability, LastSkill)
INSTELEM(abilityi)