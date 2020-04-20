#include "main.h"

INSTDATA(abilityi) = {{"Str", {}, OfStrenght},
{"Dex", {}, OfDexterity},
{"Con", {}, NoEnchant},
{"Int", {}, OfIntellegence},
{"Wis", {}, NoEnchant},
{"Cha", {}, OfCharisma},
//
{"Save vs Paralize"},
{"Save vs Poison", {}, OfPoisonResistance, 10},
{"Save vs Traps"},
{"Save vs Magic"},
//
{"Climb Walls", {Theif}},
{"Hear Noise", {Theif}},
{"Move Silently", {Theif, Ranger}},
{"Open Locks", {Theif}},
{"Remove Traps", {Theif}},
//
{"Read Languages", {Theif}},
{"Learn Spells", {Mage}},
//
{"Resist Charm"},
{"Resist Cold"},
{"Resist Fire", {}, OfFireResistance, 20},
{"Resist Magic", {}, OfMagicResistance, 10},
//
{"Deflect critical"},
{"Detect secrets"},
};
assert_enum(ability, LastSkill);