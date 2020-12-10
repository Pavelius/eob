#include "main.h"

BSDATA(damagei) = {{"Bludgeon", ResistBludgeon, {}, ImmuneNormalWeapon},
{"Slashing", ResistSlashing, {}, ImmuneNormalWeapon},
{"Pierce", ResistPierce, {}, ImmuneNormalWeapon},
{"Cold", {}, ResistCold},
{"Electricity"},
{"Fire", {}, ResistFire},
{"Magic"},
{"Heal"},
{"Paralize"},
{"Death"},
{"Petrification"},
};
assert_enum(damage, Petrification)
INSTELEM(damagei)
