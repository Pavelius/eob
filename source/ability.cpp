#include "main.h"

INSTDATA(abilityi) = {{"Str", {}, {"strenght", "ogre strenght", "hill giant", "fire giant", "cloud giant"}},
{"Dex", {}, {"archery"}},
{"Con", {}, {"health"}},
{"Int", {}, {"intellegence"}},
{"Wis", {}, {"wisdow"}},
{"Cha", {}, {"charisma"}},
//
{"Save vs Paralize"},
{"Save vs Poison", {}, {"poison immunity"}, 0, 15},
{"Save vs Traps"},
{"Save vs Magic", {}, {"magic resistance"}},
//
{"Climb Walls", {Theif}, {"feather falling"}, 0, 100},
{"Hear Noise", {Theif}},
{"Move Silently", {Theif, Ranger}, {"elvenkind"}, 0, 90},
{"Open Locks", {Theif}, {"bulgary"}, 0, 40},

{"Remove Traps", {Theif}},
//
{"Read Languages", {Theif}},
{"Learn Spells", {Mage}},
//
{"Resist Charm", {}, {"paralize immunity"}, 0, 100},
{"Resist Cold", {}, {"cold immunity"}, 0, 100},
{"Resist Fire", {}, {"fire immunity"}, 0, 100},
{"Resist Magic", {}, {"magic resistance"}, 0, 60},
//
{"Deflect critical"},
{"Detect secrets"},
};
assert_enum(ability, LastSkill);